#include "Application.h"

#include "renderer/Renderer.h"
#include "renderer/Window.h"
#include "world/Camera.h"
#include "world/Chunk.h"
#include "world/World.h"
#include "utility/ChunkAllocator.h"
#include "utility/Config.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

Application::Application()
{
	Config::Load();

	m_window = std::make_unique<Window>(WindowSettings::LoadFromConfig());
	m_renderer = std::make_unique<Renderer>(RendererSettings::LoadFromConfig(), *m_window);
	m_world = std::make_unique<World>(WorldSettings::LoadFromConfig(), *m_renderer);
}

Application::~Application()
{
	Config::Save();
}

auto Application::Run() -> void
{
	while(!glfwWindowShouldClose(static_cast<GLFWwindow*>(*m_window)))
	{
		glfwPollEvents();

		m_world->GetCamera().Position.x -= 0.1f;
		m_world->Update();
		m_renderer->Render();
	}
}
