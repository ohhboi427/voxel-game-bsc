#include "Renderer.h"

#include "Buffer.h"
#include "Shader.h"
#include "RenderTexture.h"
#include "Window.h"
#include "../world/Camera.h"
#include "../utility/Config.h"
#include "../utility/SpanUtils.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <mutex>
#include <utility>

using namespace Literals;

namespace
{
	struct ProjectionProperties
	{
		glm::mat4 View;
		glm::mat4 ViewInv;
		glm::mat4 Proj;
		glm::mat4 ProjInv;
	};

	struct ScreenProperties
	{
		glm::uvec2 Size;
	};
}

Renderer::Renderer(const RendererSettings& settings, const Window& window)
	: m_settings(settings), m_targetWindow(window)
{
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_targetWindow));
	gladLoadGL(glfwGetProcAddress);

	// Disable V-sync because on nVidia it is on by default
	glfwSwapInterval(0);

	InitializeRenderPipeline();

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(m_targetWindow), true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &m_dummyVertexArray);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

auto Renderer::InitializeRenderPipeline() -> void
{
	m_renderTexture = std::make_unique<RenderTexture>(m_targetWindow.GetSize(), GL_RGBA16F);

	m_raygenShader = std::make_unique<Shader>(
		Shader::Sources
		{
			{ GL_COMPUTE_SHADER, "res/shaders/Raygen.comp" },
		});
	m_screenShader = std::make_unique<Shader>(
		Shader::Sources
		{
			{ GL_VERTEX_SHADER, "res/shaders/Screen.vert" },
			{ GL_FRAGMENT_SHADER, "res/shaders/Screen.frag" },
		});

	m_projectionPropertiesBuffer = std::make_unique<Buffer>(
		Span(sizeof(ProjectionProperties)),
		GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	m_projectionPropertiesBuffer->Bind(GL_UNIFORM_BUFFER, 0u);

	m_screenPropertiesBuffer = std::make_unique<Buffer>(
		Span(sizeof(ScreenProperties)),
		GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	m_screenPropertiesBuffer->Bind(GL_UNIFORM_BUFFER, 1u);

	auto& screenProperties = m_screenPropertiesBuffer->GetMappedStorage<ScreenProperties>().front();
	screenProperties.Size = m_targetWindow.GetSize();

	m_chunkDataBuffer = std::make_unique<Buffer>(
		Span(m_settings.ChunkDataBufferSize),
		GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	m_chunkDataBuffer->Bind(GL_SHADER_STORAGE_BUFFER, 0u);
	m_chunkAllocator = std::make_unique<ChunkAllocator>(m_chunkDataBuffer->GetMappedStorage());

	m_dummyVertexArray;
	glCreateVertexArrays(1, &m_dummyVertexArray);
	glBindVertexArray(m_dummyVertexArray);
}

auto Renderer::UpdateProjectionData(const Camera& camera) -> void
{
	glm::quat rotation(glm::radians(camera.Rotation));

	auto& projectionProperties = m_projectionPropertiesBuffer->GetMappedStorage<ProjectionProperties>().front();
	projectionProperties.View = glm::lookAt(
		camera.Position,
		camera.Position + rotation * glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	projectionProperties.Proj = glm::perspective(
		glm::radians(camera.FieldOfView),
		static_cast<float>(m_targetWindow.GetSize().x) / static_cast<float>(m_targetWindow.GetSize().y),
		0.1f,
		1000.0f);

	projectionProperties.ViewInv = glm::inverse(projectionProperties.View);
	projectionProperties.ProjInv = glm::inverse(projectionProperties.Proj);

	GLsync bufferUploadFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
	glWaitSync(bufferUploadFence, 0, GL_TIMEOUT_IGNORED);
}

auto Renderer::BeginFrame() -> void
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

auto Renderer::EndFrame() -> void
{
	GLsync bufferUploadFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);

	m_renderTexture->Clear(glm::vec4(0.6f, 0.8f, 1.0f, 1000.0f));

	glWaitSync(bufferUploadFence, 0, GL_TIMEOUT_IGNORED);

	m_raygenShader->Use();
	{
		auto lock = std::scoped_lock(m_chunkAllocator->GetMutex());

		for(const auto& [coordinate, block] : *m_chunkAllocator)
		{
			DrawChunk(coordinate, block);
		}
	}

	m_screenShader->Use();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(static_cast<GLFWwindow*>(m_targetWindow));
}

auto Renderer::DrawChunk(const glm::ivec2& coordinate, const MemoryBlock& block) -> void
{
	glProgramUniform4i(
		static_cast<GLuint>(*m_raygenShader), DrawDataUniformLocation,
		coordinate.x, coordinate.y,
		static_cast<int32_t>(block.Offset),
		GetChunkLod(coordinate));

	glDispatchCompute(static_cast<GLuint>(m_targetWindow.GetSize().x / 8u), static_cast<GLuint>(m_targetWindow.GetSize().y / 8u), 1u);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

auto Renderer::GetChunkLod(const glm::ivec2& coordinate) const noexcept -> int32_t
{
	auto& projectionProperties = m_projectionPropertiesBuffer->GetMappedStorage<ProjectionProperties>().front();
	glm::vec3 cameraPosition = glm::vec3(projectionProperties.ViewInv[3]);

	glm::vec3 chunkPosition = glm::vec3(coordinate.x, 0.5, coordinate.y) * static_cast<float>(Chunk::Size);

	return glm::clamp<uint32_t>(static_cast<uint32_t>(glm::distance(cameraPosition, chunkPosition)) / 64, 0, 4);
}

auto RendererSettings::LoadFromConfig() -> RendererSettings
{
	return RendererSettings{
		.ChunkDataBufferSize = static_cast<size_t>(Config::Get<int64_t>("renderer", "iChunkDataBufferSize")),
	};
}
