#pragma once

#include "../utility/Octree.h"

#include <glm/glm.hpp>
#include <fastnoiselite/FastNoiseLite.h>

using Chunk = Octree<5u>;

inline auto Noise() noexcept -> FastNoiseLite&
{
	static FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

	return noise;
}

[[nodiscard]] constexpr auto GenerateChunk(const glm::uvec2& coordinate) noexcept -> Chunk
{
	Chunk chunk;

	for(uint8_t z = 0u; z < Chunk::Size; z++)
	{
		for(uint8_t x = 0u; x < Chunk::Size; x++)
		{
			glm::vec2 p = glm::vec2(x, z) + glm::vec2(coordinate) * static_cast<float>(Chunk::Size);
			p /= 4.0f;

			uint32_t h = Noise().GetNoise(p.x, p.y) * Chunk::Size;

			for(uint8_t y = 0u; y < Chunk::Size; y++)
			{
				chunk.Set(x, y, z, y <= h);
			}
		}
	}

	return chunk;
}
