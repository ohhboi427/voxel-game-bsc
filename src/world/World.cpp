#include "World.h"

#include "../renderer/Renderer.h"
#include "../utility/Config.h"

#include <glm/gtx/vec_swizzle.hpp>

#include <algorithm>
#include <future>
#include <iostream>
#include <ranges>
#include <thread>

World::World(const WorldSettings& settings, Renderer& renderer)
	: m_renderer(renderer), m_settings(settings), m_camera{
		.Position = glm::vec3(-32.0f, 64.0f, 128.0f),
		.Rotation = glm::vec3(-20.0f, -70.0f, 0.0f),
		.FieldOfView = 70.0f
	}
{}

auto World::Update() -> void
{
	glm::ivec2 cameraCoordinate = glm::ivec2(glm::xz(m_camera.Position)) / static_cast<int32_t>(Chunk::Size);

	std::unordered_set<glm::ivec2> visibleChunks;

	for(int32_t x = -m_settings.RenderDistance; x < m_settings.RenderDistance; ++x)
	{
		for(int32_t y = -m_settings.RenderDistance; y < m_settings.RenderDistance; ++y)
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

	if(!m_neededChunks.empty())
	{
		glm::ivec2& chunkCoordinate = m_neededChunks.front();

		LoadChunk(chunkCoordinate);

		m_neededChunks.pop_front();
	}

	std::erase_if(
		m_loadedChunks,
		[&] (const glm::ivec2& chunkCoordinate) -> bool
		{
			glm::ivec2 distance = glm::abs(cameraCoordinate - chunkCoordinate);

			bool shouldUnload = distance.x > m_settings.RenderDistance || distance.y > m_settings.RenderDistance;
			if(shouldUnload)
			{
				m_renderer.RemoveChunk(chunkCoordinate);
			}

			return shouldUnload;
		});

	m_renderer.SetCamera(m_camera);
}

auto World::LoadChunk(glm::ivec2 coordinate) -> void
{
	Chunk chunk = GenerateChunk(coordinate);

	m_loadedChunks.insert(coordinate);

	m_renderer.SubmitChunk(coordinate, chunk);
}

auto WorldSettings::LoadFromConfig() -> WorldSettings
{
	return WorldSettings{
		.RenderDistance = static_cast<uint8_t>(Config::Get<int64_t>("world", "iRenderDistance")),
	};
}
