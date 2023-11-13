#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <filesystem>

#include <vector>
#include <unordered_map>

#include <string>

#include <fstream>

#include <cstdint>

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

auto loadTextFile(const std::filesystem::path& path) -> std::string
{
	std::string text;

	if(std::ifstream file(path); file.good())
	{
		file.seekg(0, std::ios::end);

		if(std::streampos size = file.tellg(); size != -1)
		{
			file.seekg(0, std::ios::beg);

			text.resize(static_cast<size_t>(size));
			file.read(text.data(), static_cast<std::streamsize>(size));
		}
	}

	return text;
}

auto loadProgram(const std::unordered_map<GLenum, std::filesystem::path>& paths) -> GLuint
{
	GLuint program = glCreateProgram();

	std::vector<GLuint> shaders;
	for(const auto& [type, path] : paths)
	{
		GLuint shader = shaders.emplace_back(glCreateShader(type));

		std::string source = loadTextFile(path);
		const GLchar* sourceCStr = source.c_str();
		glShaderSource(shader, 1, &sourceCStr, nullptr);

		glCompileShader(shader);

		glAttachShader(program, shader);
	}

	glLinkProgram(program);

	for(const auto& shader : shaders)
	{
		glDetachShader(program, shader);
		glDeleteShader(shader);
	}

	return program;
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

	GLuint projectionPropertiesBuffer;
	glCreateBuffers(1, &projectionPropertiesBuffer);
	glNamedBufferStorage(projectionPropertiesBuffer, sizeof(ProjectionProperties), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0u, projectionPropertiesBuffer);
	ProjectionProperties& projectionProperties = *static_cast<ProjectionProperties*>(glMapNamedBufferRange(projectionPropertiesBuffer, 0, sizeof(ProjectionProperties), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
	projectionProperties.View = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	projectionProperties.ViewInv = glm::inverse(projectionProperties.View);
	projectionProperties.Proj = glm::perspective(glm::radians(90.0f), static_cast<float>(ScreenSize.x) / static_cast<float>(ScreenSize.y), 0.1f, 1000.0f);
	projectionProperties.ProjInv = glm::inverse(projectionProperties.Proj);

	GLuint screenPropertiesBuffer;
	glCreateBuffers(1, &screenPropertiesBuffer);
	glNamedBufferStorage(screenPropertiesBuffer, sizeof(ScreenProperties), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1u, screenPropertiesBuffer);
	ScreenProperties& screenProperties = *static_cast<ScreenProperties*>(glMapNamedBufferRange(screenPropertiesBuffer, 0, sizeof(ScreenProperties), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
	screenProperties.Size = ScreenSize;

	GLsync bufferUploadFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0u);

	GLuint renderTargetTexture;
	glCreateTextures(GL_TEXTURE_2D, 1, &renderTargetTexture);
	glTextureParameteri(renderTargetTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(renderTargetTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(renderTargetTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(renderTargetTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage2D(renderTargetTexture, 1, GL_RGBA32F, static_cast<GLsizei>(ScreenSize.x), static_cast<GLsizei>(ScreenSize.y));
	glBindTextureUnit(0u, renderTargetTexture);
	glBindImageTexture(0u, renderTargetTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	GLuint raygenProgram = loadProgram(
		{
			{ GL_COMPUTE_SHADER, "src/shaders/Raygen.comp" },
		}
	);
	glUseProgram(raygenProgram);
	glDispatchCompute(static_cast<GLuint>(ScreenSize.x / 8u), static_cast<GLuint>(ScreenSize.y / 8u), 1u);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	GLuint screenProgram = loadProgram(
		{
			{ GL_VERTEX_SHADER, "src/shaders/Screen.vert" },
			{ GL_FRAGMENT_SHADER, "src/shaders/Screen.frag" },
		}
	);
	glUseProgram(screenProgram);

	GLuint dummyVertexArray;
	glCreateVertexArrays(1, &dummyVertexArray);
	glBindVertexArray(dummyVertexArray);

	glClientWaitSync(bufferUploadFence, 0u, GL_TIMEOUT_IGNORED);

	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}

	glDeleteTextures(1, &renderTargetTexture);

	glDeleteProgram(raygenProgram);
	glDeleteProgram(screenProgram);

	glDeleteVertexArrays(1, &dummyVertexArray);

	glUnmapNamedBuffer(screenPropertiesBuffer);
	glDeleteBuffers(1, &screenPropertiesBuffer);
	glUnmapNamedBuffer(projectionPropertiesBuffer);
	glDeleteBuffers(1, &projectionPropertiesBuffer);

	glfwDestroyWindow(window);

	glfwTerminate();
}
