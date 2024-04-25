#pragma once

#include "../utility/Octree.h"

#include <glm/glm.hpp>

using Chunk = Octree<5u>;

[[nodiscard]] constexpr auto GenerateChunk(const glm::uvec2& coordinate) noexcept -> Chunk
{
	Chunk chunk;

	for(uint8_t y = 0u; y < Chunk::Size; y++)
	{
		for(uint8_t z = 0u; z < Chunk::Size; z++)
		{
			for(uint8_t x = 0u; x < Chunk::Size; x++)
			{
				chunk.Set(x, y, z, x >= y);
			}
		}
	}

	return chunk;
}
