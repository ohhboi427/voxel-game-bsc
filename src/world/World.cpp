#include "World.h"

#include "../renderer/GUI.h"
#include "../renderer/Renderer.h"
#include "../utility/Config.h"

#include <glm/gtx/vec_swizzle.hpp>
#include <imgui/imgui.h>

#include <algorithm>
#include <chrono>
#include <ranges>

#include <random>

World::World(const WorldSettings& settings, ChunkAllocator& allocator)
	: m_allocator(allocator), m_settings(settings), m_camera{
		.Position = glm::vec3(0.0f, 64.0f, 0.0f),
		.Rotation = glm::vec3(-20.0f, 70.0f, 0.0f),
		.FieldOfView = static_cast<float>(Config::Get<double>("camera", "fFieldOfView"))
	}
{
	std::random_device randomDevice;
	std::mt19937_64 randomEngine(randomDevice());
	m_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	m_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	m_noise.SetFractalOctaves(8);
	m_noise.SetFractalGain(0.5f);
	m_noise.SetFractalLacunarity(2.0f);
	m_noise.SetSeed(static_cast<int>(randomEngine()));

	GUI::OnGui += [&] (const glm::uvec2& size) -> void
		{
			ImGui::SetNextWindowSize(ImVec2(350.0f, 55.0f));
			ImGui::SetNextWindowPos(ImVec2(0.0f, 100.0f));
			ImGui::Begin("World", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			int ld = m_settings.LoadDistance;
			ImGui::SliderInt("Load Distance", &ld, 2, 16);
			m_settings.LoadDistance = ld;
			Config::Get<int64_t>("world", "iLoadDistance") = m_settings.LoadDistance;
			ImGui::End();
		};
}

World::~World()
{
	for(auto& job : m_chunkLoadingJobs)
	{
		job.wait();
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

		m_chunkLoadingJobs.emplace_back(std::move(std::async(std::launch::async, &World::LoadChunk, this, chunkCoordinate)));

		m_neededChunks.pop_front();
	}

	// Remove chunks that have been loaded
	std::erase_if(
		m_loadedChunks,
		[&] (const glm::ivec2& chunkCoordinate) -> bool
		{
			if(!visibleChunks.contains(chunkCoordinate))
			{
				m_allocator.Free(chunkCoordinate);

				return true;
			}

			return false;
		});

	// Remove the jobs that are finished
	std::erase_if(
		m_chunkLoadingJobs,
		[] (const std::future<void>& job) -> bool
		{
			return job.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
		});
}

auto World::LoadChunk(glm::ivec2 coordinate) -> void
{
	Chunk chunk = GenerateChunk(coordinate);
	if(m_allocator.Allocate(coordinate, chunk))
	{
		m_loadedChunks.insert(coordinate);
	}
}

auto World::GenerateChunk(const glm::ivec2& coordinate) const -> Chunk
{
	Chunk chunk;

	for(uint8_t z = 0u; z < Chunk::Size; z++)
	{
		for(uint8_t x = 0u; x < Chunk::Size; x++)
		{
			glm::vec2 p = glm::vec2(x, z) + glm::vec2(coordinate) * static_cast<float>(Chunk::Size);
			p /= 2.0f;

			uint8_t h = static_cast<uint8_t>(((m_noise.GetNoise(p.x, p.y) + 1.0f) / 2.0f) * Chunk::Size);

			for(uint8_t y = 0u; y < Chunk::Size; y++)
			{
				chunk.Set(glm::uvec3(x, y, z), y <= h);
			}
		}
	}

	return chunk;
}

auto WorldSettings::LoadFromConfig() -> WorldSettings
{
	return WorldSettings{
		.LoadDistance = static_cast<uint8_t>(Config::Get<int64_t>("world", "iLoadDistance")),
	};
}
