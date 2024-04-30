#pragma once

#include "Camera.h"
#include "Chunk.h"

#include <deque>
#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

class Renderer;

/**
 * @brief Holds settings related to a world.
 */
struct WorldSettings
{
	/**
	 * @brief The number of chunks visible from the camera in one direction.
	 */
	uint8_t RenderDistance;

	/**
	 * @brief Loads the settings from the config file.
	 * 
	 * @return The settings loaded from the file.
	 */
	static auto LoadFromConfig() -> WorldSettings;
};

class World
{
public:
	World(const WorldSettings& settings, Renderer& renderer);

	auto Update() -> void;

	[[nodiscard]] auto GetCamera() noexcept -> Camera&
	{
		return m_camera;
	}

	[[nodiscard]] auto GetCamera() const noexcept -> const Camera&
	{
		return m_camera;
	}

private:
	Camera m_camera;
	Renderer& m_renderer;
	WorldSettings m_settings;
	std::deque<glm::ivec2> m_neededChunks;
	std::unordered_set<glm::ivec2> m_loadedChunks;

	/**
	 * @brief Generates a chunk.
	 * 
	 * @param coordinate The coordinate of the chunk.
	 */
	auto LoadChunk(glm::ivec2 coordinate) -> void;
};
