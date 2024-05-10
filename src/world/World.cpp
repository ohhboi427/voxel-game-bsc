#include "World.h"

#include "../renderer/Renderer.h"
#include "../utility/Config.h"

#include <glm/gtx/vec_swizzle.hpp>

#include <algorithm>
#include <ranges>

World::World(const WorldSettings& settings, ChunkAllocator& allocator)
	: m_allocator(allocator), m_settings(settings), m_camera{
		.Position = glm::vec3(-32.0f, 64.0f, 128.0f),
		.Rotation = glm::vec3(-20.0f, 70.0f, 0.0f),
		.FieldOfView = 70.0f
	}
{}

World::~World()
{
	for(auto& job : m_chunkLoadingJobs)
	{
		job.join();
	}
}

auto World::Update() -> void
{
	glm::ivec2 cameraCoordinate = glm::ivec2(glm::xz(m_camera.Position)) / static_cast<int32_t>(Chunk::Size);

	std::unordered_set<glm::ivec2> visibleChunks;
	for(int32_t x = -m_settings.LoadDistance; x < m_settings.LoadDistance; ++x)
	{
		for(int32_t y = -m_settings.LoadDistance; y < m_settings.LoadDistance; ++y)
		{
			glm::ivec2 chunkCoordinate = glm::ivec2(x, y) + cameraCoordinate;

			visibleChunks.insert(chunkCoordinate);
			if(
				!m_loadedChunks.contains(chunkCoordinate) &&
				std::ranges::find(m_neededChunks, chunkCoordinate) == m_neededChunks.end())
			{
				m_neededChunks.push_back(chunkCoordinate);
			}
		}
	}

	// Lunch one job per frame
	if(!m_neededChunks.empty())
	{
		glm::ivec2& chunkCoordinate = m_neededChunks.front();

		m_chunkLoadingJobs.emplace_back(&World::LoadChunk, this, chunkCoordinate);

		m_neededChunks.pop_front();
	}

	// Remove chunks that have been loaded
	std::erase_if(
		m_loadedChunks,
		[&] (const glm::ivec2& chunkCoordinate) -> bool
		{
			glm::ivec2 distance = glm::abs(cameraCoordinate - chunkCoordinate);

			bool shouldUnload = distance.x > m_settings.LoadDistance || distance.y > m_settings.LoadDistance;
			if(shouldUnload)
			{
				m_allocator.Free(chunkCoordinate);
			}

			return shouldUnload;
		});

	// Remove the jobs that are finished
	std::erase_if(
		m_chunkLoadingJobs,
		[] (const std::thread& thread) -> bool
		{
			return !thread.joinable();
		});
}

auto World::LoadChunk(glm::ivec2 coordinate) -> void
{
	Chunk chunk = GenerateChunk(coordinate);

	m_loadedChunks.insert(coordinate);

	m_allocator.Allocate(coordinate, chunk);
}

auto WorldSettings::LoadFromConfig() -> WorldSettings
{
	return WorldSettings{
		.LoadDistance = static_cast<uint8_t>(Config::Get<int64_t>("world", "iLoadDistance")),
	};
}
