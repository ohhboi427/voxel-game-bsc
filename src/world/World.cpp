#include "World.h"

#include "../renderer/Renderer.h"

#include <glm/gtx/vec_swizzle.hpp>

#include <future>
#include <iostream>
#include <thread>

World::World(Renderer& renderer)
	: m_renderer(renderer), m_camera{
		.Position = glm::vec3(-32.0f, 64.0f, 128.0f),
		.Rotation = glm::vec3(-20.0f, -70.0f, 0.0f),
		.FieldOfView = 70.0f
	}
{}

auto World::Update() -> void
{
	glm::ivec2 cameraCoordinate = glm::ivec2(glm::xz(m_camera.Position)) / static_cast<int32_t>(Chunk::Size);

	std::unordered_set<glm::ivec2> visibleChunks;

	for(int32_t x = -4; x < 4; ++x)
	{
		for(int32_t y = -4; y < 4; ++y)
		{
			glm::ivec2 chunkCoordinate = glm::ivec2(x, y) + cameraCoordinate;

			visibleChunks.insert(chunkCoordinate);
			if(!m_loadedChunks.contains(chunkCoordinate))
			{
				GenerateChunk(chunkCoordinate);
			}
		}
	}

	std::erase_if(
		m_loadedChunks,
		[&] (const glm::ivec2& chunkCoordinate) -> bool
		{
			glm::ivec2 distance = glm::abs(cameraCoordinate - chunkCoordinate);

			bool shouldUnload = distance.x > 4 || distance.y > 4;
			if(shouldUnload)
			{
				m_renderer.RemoveChunk(chunkCoordinate);
			}

			return shouldUnload;
		});

	m_renderer.SetCamera(m_camera);
}

auto World::GenerateChunk(glm::ivec2 coordinate) -> void
{
	Chunk chunk = ::GenerateChunk(coordinate);

	m_loadedChunks.insert(coordinate);

	OnChunkGenerated(coordinate, chunk);
}

auto World::OnChunkGenerated(const glm::ivec2& coordinate, const Chunk& chunk) -> void
{
	m_renderer.SubmitChunk(coordinate, chunk);
}
