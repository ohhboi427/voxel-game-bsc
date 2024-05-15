#include "Application.h"

#include "renderer/Renderer.h"
#include "renderer/Window.h"
#include "world/Camera.h"
#include "world/Chunk.h"
#include "world/World.h"
#include "utility/ChunkAllocator.h"
#include "utility/Config.h"
#include "utility/Input.h"
#include "utility/Time.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui/imgui.h>

Application::Application()
{
	Config::Load();

	m_window = std::make_unique<Window>(WindowSettings::LoadFromConfig());
	m_renderer = std::make_unique<Renderer>(RendererSettings::LoadFromConfig(), *m_window);

	m_world = std::make_unique<World>(WorldSettings::LoadFromConfig(), m_renderer->GetChunkAllocator());

	Input::Initialize(*m_window);
}

Application::~Application()
{
	Config::Save();
}

auto Application::Run() -> void
{
	glm::vec3 cameraMovement(0.0f);
	Input::OnKey += [&] (int key, bool isPressed) -> void
		{
			static constexpr float Speed = 10.0f;

			float dir = (isPressed)
				? 1.0f
				: -1.0f;

			switch(key)
			{
			case GLFW_KEY_D:
				cameraMovement.x += dir * Speed;
				break;
			case GLFW_KEY_A:
				cameraMovement.x -= dir * Speed;
				break;
			case GLFW_KEY_S:
				cameraMovement.z += dir * Speed;
				break;
			case GLFW_KEY_W:
				cameraMovement.z -= dir * Speed;
				break;
			}
		};

	glm::vec2 cameraRotation(0.0f);
	Input::OnCursorMove += [&] (const glm::vec2& current, const glm::vec2& delta) -> void
		{
			static constexpr float MouseSpeed = 0.25f;

			cameraRotation.y -= delta.x * MouseSpeed;
			cameraRotation.x -= delta.y * MouseSpeed;
			cameraRotation.x = glm::clamp(cameraRotation.x, -89.999f, 89.999f);
		};

	Time::Reset();
	while(!glfwWindowShouldClose(static_cast<GLFWwindow*>(*m_window)))
	{
		Input::Poll();
		Time::Tick();

		Camera& camera = m_world->GetCamera();
		camera.Rotation = glm::vec3(cameraRotation, 0.0f);
		camera.Position += glm::quat(glm::radians(camera.Rotation)) * cameraMovement * Time::GetDeltaTime();

		m_renderer->BeginFrame();

		m_renderer->UpdateProjectionData(camera);
		m_world->Update();

		m_renderer->EndFrame();
	}
}
