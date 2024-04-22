#include "Renderer.h"

#include "Buffer.h"
#include "Shader.h"
#include "../world/Chunk.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

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

Renderer::Renderer(const glm::uvec2& screenSize, GLFWwindow* window)
	: m_window(window)
{
	gladLoadGL(glfwGetProcAddress);

	m_projectionPropertiesBuffer = std::make_unique<Buffer>(sizeof(ProjectionProperties), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	m_projectionPropertiesBuffer->Bind(GL_UNIFORM_BUFFER, 0u);
	auto& projectionProperties = *m_projectionPropertiesBuffer->GetMappedStorage<ProjectionProperties>();
	projectionProperties.View = glm::lookAt(glm::vec3(-16.0f, 48.0f, 72.0f), glm::vec3(16.0f, 16.0f, 16.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	projectionProperties.ViewInv = glm::inverse(projectionProperties.View);
	projectionProperties.Proj = glm::perspective(glm::radians(70.0f), static_cast<float>(screenSize.x) / static_cast<float>(screenSize.y), 0.1f, 1000.0f);
	projectionProperties.ProjInv = glm::inverse(projectionProperties.Proj);

	m_screenPropertiesBuffer = std::make_unique<Buffer>(sizeof(ScreenProperties), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	m_screenPropertiesBuffer->Bind(GL_UNIFORM_BUFFER, 1u);
	auto& screenProperties = *m_screenPropertiesBuffer->GetMappedStorage<ScreenProperties>();
	screenProperties.Size = screenSize;

	GLsync bufferUploadFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);

	m_chunkDataBuffer = std::make_unique<Buffer>(134217728, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	m_chunkDataBuffer->Bind(GL_SHADER_STORAGE_BUFFER, 0u);

	m_renderTexture;
	glCreateTextures(GL_TEXTURE_2D, 1, &m_renderTexture);
	glTextureParameteri(m_renderTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_renderTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_renderTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_renderTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage2D(m_renderTexture, 1, GL_RGBA16F, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
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

	glWaitSync(bufferUploadFence, 0, GL_TIMEOUT_IGNORED);
}

Renderer::~Renderer()
{
	glDeleteTextures(1, &m_renderTexture);

	glDeleteVertexArrays(1, &m_dummyVertexArray);
}

auto Renderer::SubmitChunk(const glm::uvec2& coordinate, const Chunk& chunk) -> void
{
	memcpy(m_chunkDataBuffer->GetMappedStorage(), chunk.Data(), chunk.DataSize());

	GLsync bufferUploadFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);
	glWaitSync(bufferUploadFence, 0, GL_TIMEOUT_IGNORED);
}

auto Renderer::RemoveChunk(const glm::uvec2& coordinate) -> void
{}

auto Renderer::Render() -> void
{
	auto& screenProperties = *m_screenPropertiesBuffer->GetMappedStorage<ScreenProperties>();

	m_raygenShader->Use();
	glDispatchCompute(static_cast<GLuint>(screenProperties.Size.x / 8u), static_cast<GLuint>(screenProperties.Size.y / 8u), 1u);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	m_screenShader->Use();
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glfwSwapBuffers(m_window);
}
