#include "Application.h"

#include "renderer/Renderer.h"
#include "renderer/Window.h"
#include "world/Camera.h"
#include "world/Chunk.h"
#include "world/World.h"
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
	World world(*m_renderer);

	m_renderer->SetCamera(
		Camera{
			.Position = glm::vec3(-32.0f, 64.0f, 128.0f),
			.Rotation = glm::vec3(-20.0f, -70.0f, 0.0f),
			.FieldOfView = 70.0f,
		});

	while(!glfwWindowShouldClose(static_cast<GLFWwindow*>(*m_window)))
	{
		glfwPollEvents();

		world.GetCamera().Position.x -= 0.1f;
		world.Update();
		m_renderer->Render();
	}
}
