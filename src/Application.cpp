#include "Application.h"

#include "renderer/Renderer.h"
#include "renderer/Window.h"
#include "world/Camera.h"
#include "world/Chunk.h"

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
	static constexpr auto ChunkCoordinate = glm::uvec2(0u, 0u);

	Chunk chunk = GenerateChunk(ChunkCoordinate);
	m_renderer->SubmitChunk(ChunkCoordinate, chunk);

	/*m_renderer->SetCamera(
		Camera{
			.Position = glm::vec3(-16.0f, 48.0f, 48.0f),
			.Rotation = glm::vec3(-30.0f, -45.0f, 0.0f),
			.FieldOfView = 70.0f,
		});
	*/

	while(!glfwWindowShouldClose(static_cast<GLFWwindow*>(*m_window)))
	{
		glfwPollEvents();

		m_renderer->Render();
	}
}
