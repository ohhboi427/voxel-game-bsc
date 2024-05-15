#include "CameraController.h"

#include "../renderer/GUI.h"
#include "../world/Camera.h"
#include "../utility/Input.h"
#include "../utility/Time.h"

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <imgui/imgui.h>

CameraController::CameraController(Camera& camera)
	: m_camera(camera), m_movementSpeed(10.0f), m_rotationSpeed(0.25f), m_movement(0.0f), m_rotation(0.0f)
{
	Input::OnKey += [&] (int key, bool isPressed) -> void
		{
			if(GUI::IsVisible)
			{
				return;
			}

			float dir = (isPressed)
				? 1.0f
				: -1.0f;

			switch(key)
			{
			case GLFW_KEY_D:
				m_movement.x += dir * m_movementSpeed;
				break;
			case GLFW_KEY_A:
				m_movement.x -= dir * m_movementSpeed;
				break;
			case GLFW_KEY_S:
				m_movement.z += dir * m_movementSpeed;
				break;
			case GLFW_KEY_W:
				m_movement.z -= dir * m_movementSpeed;
				break;
			}
		};

	Input::OnCursorMove += [&] (const glm::vec2& current, const glm::vec2& delta) -> void
		{
			if(GUI::IsVisible)
			{
				return;
			}

			m_rotation.y -= delta.x * m_rotationSpeed;
			m_rotation.x -= delta.y * m_rotationSpeed;
			m_rotation.x = glm::clamp(m_rotation.x, -89.999f, 89.999f);
		};

	GUI::OnGui += [&] (const glm::uvec2& windowSize) -> void
		{
			ImGui::SetNextWindowSize(ImVec2(static_cast<float>(windowSize.x), 65.0f));
			ImGui::SetNextWindowPos(ImVec2(0.0f, static_cast<float>(windowSize.y - 65u)));
			ImGui::Begin("Guide", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			ImGui::Text("Press the 'ESC' button to toggle between camera movement and GUI.");
			ImGui::Text("Use the 'W', 'A', 'S' and 'D' keys to move the camera relative to its rotation. You can rotate the camera using the mouse.");
			ImGui::End();

			ImGui::SetNextWindowSize(ImVec2(350.0f, 80.0f));
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			ImGui::DragFloat("Movement speed", &m_movementSpeed, 0.5f, 1.0f, 20.0f, "%.2f");
			ImGui::DragFloat("Rotation Speed", &m_rotationSpeed, 0.01f, 0.01f, 1.0f, "%.2f");
			ImGui::End();
		};
}

auto CameraController::Update() -> void
{
	m_camera.Rotation = glm::vec3(m_rotation, 0.0f);
	m_camera.Position += glm::quat(glm::radians(m_camera.Rotation)) * m_movement * Time::GetDeltaTime();
}
