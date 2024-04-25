#include "Application.h"

#include "renderer/Renderer.h"
#include "renderer/Window.h"
#include "world/Camera.h"
#include "world/Chunk.h"
#include "utility/ChunkAllocator.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

Application::Application()
{
	m_window = std::make_unique<Window>(glm::uvec2(1280u, 720u), "voxel-game");
	m_renderer = std::make_unique<Renderer>(*m_window);
}

Application::~Application()
{

}

auto Application::Run() -> void
{
	for(int32_t x = 0; x <= 4; x++)
	{
		for(int32_t y = 0; y <= 4; y++)
		{
			glm::uvec2 coordinate(x, y);

			Chunk chunk = GenerateChunk(coordinate);
			m_renderer->SubmitChunk(coordinate, chunk);
		}
	}

	m_renderer->SetCamera(
		Camera{
			.Position = glm::vec3(-32.0f, 64.0f, 128.0f),
			.Rotation = glm::vec3(-20.0f, -70.0f, 0.0f),
			.FieldOfView = 70.0f,
		});

	while(!glfwWindowShouldClose(static_cast<GLFWwindow*>(*m_window)))
	{
		glfwPollEvents();

		m_renderer->Render();
	}
}
