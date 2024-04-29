#pragma once

#include "Camera.h"
#include "Chunk.h"

#include <deque>
#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

class Renderer;

class World
{
public:
	World(Renderer& renderer);

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
	std::deque<glm::ivec2> m_neededChunks;
	std::unordered_set<glm::ivec2> m_loadedChunks;

	/**
	 * @brief Generates a chunk.
	 * 
	 * @param coordinate The coordinate of the chunk.
	 */
	auto GenerateChunk(glm::ivec2 coordinate) -> void;

	/**
	 * @brief Submits a chunk to the renderer after it has been generated.
	 * 
	 * @param coordinate The coordinate of the chunk.
	 * @param chunk The chunk.
	 */
	auto OnChunkGenerated(const glm::ivec2& coordinate, const Chunk& chunk) -> void;
};
