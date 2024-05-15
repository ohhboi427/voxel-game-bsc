#pragma once

#include <glm/glm.hpp>

struct Camera;

/**
 * @brief Handles input to move the camera.
 */
class CameraController
{
public:
	/**
	 * @brief Sets the controlled camera.
	 * 
	 * @param camera The controlled camera.
	 */
	CameraController(Camera& camera);

	/**
	 * @brief Updates the camers's transform.
	 */
	auto Update() -> void;

private:
	Camera& m_camera;
	float m_movementSpeed;
	float m_rotationSpeed;
	glm::vec3 m_movement;
	glm::vec2 m_rotation;
};
