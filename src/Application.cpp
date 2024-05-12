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
#include <glm/ext.hpp>

Application::Application()
{
	Config::Load();

	m_window = std::make_unique<Window>(WindowSettings::LoadFromConfig());
	m_renderer = std::make_unique<Renderer>(RendererSettings::LoadFromConfig(), *m_window);

	m_world = std::make_unique<World>(WorldSettings::LoadFromConfig(), m_renderer->GetChunkAllocator());
}

Application::~Application()
{
	Config::Save();
}

auto Application::Run() -> void
{
	static constexpr float Speed = 10.0f;
	static constexpr float MouseSpeed = 0.25f;

	Camera& camera = m_world->GetCamera();

	float current = static_cast<float>(glfwGetTime());
	glm::dvec2 currentMouse;
	glfwGetCursorPos(static_cast<GLFWwindow*>(*m_window), &currentMouse.x, &currentMouse.y);
	while(!glfwWindowShouldClose(static_cast<GLFWwindow*>(*m_window)))
	{
		glfwPollEvents();

		float last = current;
		current = static_cast<float>(glfwGetTime());
		float dt = current - last;

		glm::dvec2 lastMouse = currentMouse;
		glfwGetCursorPos(static_cast<GLFWwindow*>(*m_window), &currentMouse.x, &currentMouse.y);
		glm::vec2 dm(currentMouse - lastMouse);

		camera.Rotation.y -= dm.x * MouseSpeed;
		camera.Rotation.x -= dm.y * MouseSpeed;
		camera.Rotation.x = glm::clamp(camera.Rotation.x, -89.999f, 89.999f);

		glm::vec3 movement(0.0f);
		if(glfwGetKey(static_cast<GLFWwindow*>(*m_window), GLFW_KEY_D))
		{
			movement.x += 1.0f;
		}
		if(glfwGetKey(static_cast<GLFWwindow*>(*m_window), GLFW_KEY_A))
		{
			movement.x -= 1.0f;
		}
		if(glfwGetKey(static_cast<GLFWwindow*>(*m_window), GLFW_KEY_W))
		{
			movement.z -= 1.0f;
		}
		if(glfwGetKey(static_cast<GLFWwindow*>(*m_window), GLFW_KEY_S))
		{
			movement.z += 1.0f;
		}
		if(movement != glm::vec3(0.0f))
		{
			glm::quat rotation = glm::radians(camera.Rotation);

			movement = glm::normalize(movement) * Speed;
			camera.Position += rotation * movement * dt;
		}


		m_renderer->UpdateProjectionData(camera);
		m_world->Update();
		m_renderer->Render();
	}
}
