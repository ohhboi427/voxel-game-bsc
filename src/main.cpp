#include "renderer/Buffer.h"
#include "renderer/Shader.h"

#include "utility/Octree.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

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

using Chunk = Octree<5u>;

auto GenerateVoxelData(Chunk& chunk) -> void
{
	for(uint8_t y = 0u; y < Chunk::Size; y++)
	{
		for(uint8_t z = 0u; z < Chunk::Size; z++)
		{
			for(uint8_t x = 0u; x < Chunk::Size; x++)
			{
				chunk.Set(x, y, z, x >= y);
			}
		}
	}
}

auto main(int argc, char* argv[]) -> int
{
	static constexpr glm::uvec2 ScreenSize = glm::uvec2(1280u, 720u);

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(static_cast<int>(ScreenSize.x), static_cast<int>(ScreenSize.y), "voxel-game", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);

	Buffer projectionPropertiesBuffer(sizeof(ProjectionProperties), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	projectionPropertiesBuffer.Bind(GL_UNIFORM_BUFFER, 0u);
	auto& projectionProperties = *projectionPropertiesBuffer.GetMappedStorage<ProjectionProperties>();
	projectionProperties.View = glm::lookAt(glm::vec3(-16.0f, 48.0f, 72.0f), glm::vec3(16.0f, 16.0f, 16.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	projectionProperties.ViewInv = glm::inverse(projectionProperties.View);
	projectionProperties.Proj = glm::perspective(glm::radians(70.0f), static_cast<float>(ScreenSize.x) / static_cast<float>(ScreenSize.y), 0.1f, 1000.0f);
	projectionProperties.ProjInv = glm::inverse(projectionProperties.Proj);

	Buffer screenPropertiesBuffer(sizeof(ScreenProperties), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	screenPropertiesBuffer.Bind(GL_UNIFORM_BUFFER, 1u);
	auto& screenProperties = *screenPropertiesBuffer.GetMappedStorage<ScreenProperties>();
	screenProperties.Size = ScreenSize;

	// Buffer voxelDataBuffer(sizeof(Chunk), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	// voxelDataBuffer.Bind(GL_SHADER_STORAGE_BUFFER, 0u);
	// auto& voxelData = *voxelDataBuffer.GetMappedStorage<Chunk>();
	// GenerateVoxelData(voxelData);

	Chunk chunk;
	GenerateVoxelData(chunk);
	Buffer voxelDataBuffer(chunk.DataSize(), chunk.Data());
	voxelDataBuffer.Bind(GL_SHADER_STORAGE_BUFFER, 0u);

	GLsync bufferUploadFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);

	GLuint renderTargetTexture;
	glCreateTextures(GL_TEXTURE_2D, 1, &renderTargetTexture);
	glTextureParameteri(renderTargetTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(renderTargetTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(renderTargetTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(renderTargetTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage2D(renderTargetTexture, 1, GL_RGBA16F, static_cast<GLsizei>(ScreenSize.x), static_cast<GLsizei>(ScreenSize.y));
	glBindTextureUnit(0u, renderTargetTexture);
	glBindImageTexture(0u, renderTargetTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

	Shader raygenShader({
		{ GL_COMPUTE_SHADER, "res/shaders/Raygen.comp" },
		});

	Shader screenShader({
		{ GL_VERTEX_SHADER, "res/shaders/Screen.vert" },
		{ GL_FRAGMENT_SHADER, "res/shaders/Screen.frag" },
		});

	GLuint dummyVertexArray;
	glCreateVertexArrays(1, &dummyVertexArray);
	glBindVertexArray(dummyVertexArray);

	glClientWaitSync(bufferUploadFence, 0u, GL_TIMEOUT_IGNORED);

	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		raygenShader.Use();
		glDispatchCompute(static_cast<GLuint>(ScreenSize.x / 8u), static_cast<GLuint>(ScreenSize.y / 8u), 1u);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		screenShader.Use();
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}

	glDeleteTextures(1, &renderTargetTexture);

	glDeleteVertexArrays(1, &dummyVertexArray);

	glfwDestroyWindow(window);

	glfwTerminate();
}
