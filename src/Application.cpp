#include "Application.h"

#include "world/Chunk.h"
#include "renderer/Renderer.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

static constexpr glm::uvec2 ScreenSize = glm::uvec2(1280u, 720u);

Application::Application()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(static_cast<int>(ScreenSize.x), static_cast<int>(ScreenSize.y), "voxel-game", nullptr, nullptr);

	glfwMakeContextCurrent(m_window);

	m_renderer = std::make_unique<Renderer>(ScreenSize, m_window);
}

Application::~Application()
{
	glfwDestroyWindow(m_window);

	glfwTerminate();
}

auto Application::Run() -> void
{
	static constexpr auto ChunkCoordinate = glm::uvec2(0u, 0u);

	Chunk chunk = GenerateChunk(ChunkCoordinate);
	m_renderer->SubmitChunk(ChunkCoordinate, chunk);

	while(!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();

		m_renderer->Render();
	}
}
