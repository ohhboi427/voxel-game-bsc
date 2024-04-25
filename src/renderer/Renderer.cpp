#include "Renderer.h"

#include "Buffer.h"
#include "Shader.h"
#include "Window.h"
#include "../world/Camera.h"
#include "../world/Chunk.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

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

Renderer::Renderer(const Window& window)
	: m_window(window)
{
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_window));
	gladLoadGL(glfwGetProcAddress);

	InitializeProjectionData();
	GLsync projectionDataFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);

	InitializeChunkData();

	m_renderTexture;
	glCreateTextures(GL_TEXTURE_2D, 1, &m_renderTexture);
	glTextureParameteri(m_renderTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_renderTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_renderTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_renderTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage2D(m_renderTexture, 1, GL_RGBA16F, static_cast<GLsizei>(m_window.GetSize().x), static_cast<GLsizei>(m_window.GetSize().y));
	glBindTextureUnit(0u, m_renderTexture);
	glBindImageTexture(0u, m_renderTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

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

	m_dummyVertexArray;
	glCreateVertexArrays(1, &m_dummyVertexArray);
	glBindVertexArray(m_dummyVertexArray);

	glWaitSync(projectionDataFence, 0, GL_TIMEOUT_IGNORED);

	glfwSwapInterval(0);
}

Renderer::~Renderer()
{
	glDeleteTextures(1, &m_renderTexture);

	glDeleteVertexArrays(1, &m_dummyVertexArray);
}

auto Renderer::SubmitChunk(const glm::uvec2& coordinate, const Chunk& chunk) -> void
{
	MemoryBlock block = m_chunkAllocator->Allocate(chunk.Data(), chunk.DataSize());
	m_submittedChunks.insert_or_assign(coordinate, block);

	GLsync bufferUploadFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
	glWaitSync(bufferUploadFence, 0, GL_TIMEOUT_IGNORED);
}

auto Renderer::RemoveChunk(const glm::uvec2& coordinate) -> void
{
	MemoryBlock block = m_submittedChunks.at(coordinate);
	m_chunkAllocator->Free(block);
}

auto Renderer::SetCamera(const Camera& camera) -> void
{
	glm::quat rotation(glm::radians(camera.Rotation));

	auto& projectionProperties = *m_projectionPropertiesBuffer->GetMappedStorage<ProjectionProperties>();
	projectionProperties.View = glm::lookAt(
		camera.Position,
		camera.Position + rotation * glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	projectionProperties.Proj = glm::perspective(
		glm::radians(camera.FieldOfView),
		static_cast<float>(m_window.GetSize().x) / static_cast<float>(m_window.GetSize().y),
		0.1f,
		1000.0f);

	projectionProperties.ViewInv = glm::inverse(projectionProperties.View);
	projectionProperties.ProjInv = glm::inverse(projectionProperties.Proj);
}

auto Renderer::Render() -> void
{
	static constexpr glm::vec4 Clear(0.6f, 0.8f, 1.0f, 1000.0f);
	glClearTexImage(m_renderTexture, 0u, GL_RGBA, GL_FLOAT, &Clear);

	m_raygenShader->Use();
	for(const auto& [coordinate, block] : m_submittedChunks)
	{
		DrawChunk(coordinate, block);
	}

	m_screenShader->Use();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glfwSwapBuffers(static_cast<GLFWwindow*>(m_window));
}

auto Renderer::InitializeChunkData() -> void
{
	m_chunkDataBuffer = std::make_unique<Buffer>(128_mb, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	m_chunkDataBuffer->Bind(GL_SHADER_STORAGE_BUFFER, 0u);
	m_chunkAllocator = std::make_unique<ChunkAllocator>(m_chunkDataBuffer->GetMappedStorage(), 64_mb);
}

auto Renderer::InitializeProjectionData() -> void
{
	m_projectionPropertiesBuffer = std::make_unique<Buffer>(sizeof(ProjectionProperties), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	m_projectionPropertiesBuffer->Bind(GL_UNIFORM_BUFFER, 0u);
	SetCamera(
		Camera
		{
			.Position = glm::vec3(-16.0f, 32.0f, 48.0f),
			.Rotation = glm::vec3(-25.0f, -45.0f, 0.0f),
			.FieldOfView = 70.0f,
		});

	m_screenPropertiesBuffer = std::make_unique<Buffer>(sizeof(ScreenProperties), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	m_screenPropertiesBuffer->Bind(GL_UNIFORM_BUFFER, 1u);

	auto& screenProperties = *m_screenPropertiesBuffer->GetMappedStorage<ScreenProperties>();
	screenProperties.Size = m_window.GetSize();
}

auto Renderer::DrawChunk(const glm::uvec2& coordinate, const MemoryBlock& block) -> void
{
	glProgramUniform4ui(
		static_cast<GLuint>(*m_raygenShader), DrawDataLocation,
		coordinate.x, coordinate.y,
		static_cast<uint32_t>(block.Offset),
		GetChunkLod(coordinate));

	glDispatchCompute(static_cast<GLuint>(m_window.GetSize().x / 8u), static_cast<GLuint>(m_window.GetSize().y / 8u), 1u);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

auto Renderer::GetChunkLod(const glm::uvec2& coordinate) const noexcept -> uint32_t
{
	auto& projectionProperties = *m_projectionPropertiesBuffer->GetMappedStorage<ProjectionProperties>();
	glm::vec3 cameraPosition = glm::vec3(projectionProperties.ViewInv[3].x, projectionProperties.ViewInv[3].y, projectionProperties.ViewInv[3].z);

	glm::vec3 chunkPosition = glm::vec3(coordinate.x, 0.5, coordinate.y) * float(Chunk::Size);

	return glm::clamp<uint32_t>(static_cast<uint32_t>(glm::distance(cameraPosition, chunkPosition)) / 64, 0, 4);
}
