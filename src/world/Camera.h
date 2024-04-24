#pragma once

#include <glm/glm.hpp>

/**
 * @brief Camera data.
 */
struct Camera
{
	/**
	 * @brief The position of the camera.
	 */
	glm::vec3 Position;

	/**
	 * @brief The euler angles of the camera in degrees.
	 */
	glm::vec3 Rotation;

	/**
	 * @brief The vertical field of view of the camera in degrees.
	 */
	float FieldOfView;
};
