#include "Application.h"

#include "renderer/GUI.h"
#include "renderer/Renderer.h"
#include "renderer/Window.h"
#include "world/Camera.h"
#include "world/Chunk.h"
#include "world/World.h"
#include "utility/ChunkAllocator.h"
#include "utility/Config.h"
#include "utility/Input.h"
#include "utility/Time.h"
#include "scripts/CameraController.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui/imgui.h>

Application::Application()
{
	Config::Load();

	m_window = std::make_unique<Window>(WindowSettings::LoadFromConfig());
	m_renderer = std::make_unique<Renderer>(RendererSettings::LoadFromConfig(), *m_window);
	Input::Initialize(*m_window);
	GUI::Initialize(*m_window);

	m_world = std::make_unique<World>(WorldSettings::LoadFromConfig(), m_renderer->GetChunkAllocator());

	m_cameraController = std::make_unique<CameraController>(m_world->GetCamera());
}

Application::~Application()
{
	GUI::Destroy();
	Config::Save();
}

auto Application::Run() -> void
{
	Time::Reset();
	while(!m_window->GetShouldClose())
	{
		Time::Tick();
		Input::Poll();

		m_renderer->BeginFrame();

		if(GUI::IsVisible)
		{
			GUI::OnGui(m_window->GetSize());
		}

		m_cameraController->Update();

		m_renderer->UpdateProjectionData(m_world->GetCamera());
		m_world->Update();

		m_renderer->EndFrame();
	}
}
