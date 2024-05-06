#pragma once

#include "Camera.h"
#include "Chunk.h"

#include <deque>
#include <thread>
#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

class ChunkAllocator;

/**
 * @brief Holds settings related to a world.
 */
struct WorldSettings
{
	/**
	 * @brief The number of chunks visible from the camera in one direction.
	 */
	uint8_t LoadDistance;

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
	/**
	 * @brief Sets the settings and the allocator.
	 * 
	 * @param settings The settings of the world.
	 * @param allocator The allocator used to allocate memory for the chunks.
	 */
	World(const WorldSettings& settings, ChunkAllocator& allocator);

	/**
	 * @brief Waits for the chunk generation jobs to finish.
	 */
	~World();

	/**
	 * @brief Updates the loaded chunks.
	 */
	auto Update() -> void;

	/**
	 * @brief Retrieves the world's camera.
	 * 
	 * @return A reference to the camera.
	 */
	[[nodiscard]] auto GetCamera() noexcept -> Camera&
	{
		return m_camera;
	}

	/**
	 * @brief Retrieves the world's camera.
	 * 
	 * @return A const reference to the camera.
	 */
	[[nodiscard]] auto GetCamera() const noexcept -> const Camera&
	{
		return m_camera;
	}

private:
	WorldSettings m_settings;
	Camera m_camera;
	ChunkAllocator& m_allocator;
	std::deque<glm::ivec2> m_neededChunks;
	std::vector<std::thread> m_chunkLoadingJobs;
	std::unordered_set<glm::ivec2> m_loadedChunks;

	/**
	 * @brief Generates a chunk.
	 * 
	 * @param coordinate The coordinate of the chunk.
	 */
	auto LoadChunk(glm::ivec2 coordinate) -> void;
};
