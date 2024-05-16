#pragma once

#include "../world/Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <mutex>
#include <span>
#include <unordered_map>
#include <vector>

/**
 * @brief Memory block descriptor.
 */
struct MemoryBlock
{
	/**
	 * @brief The memory block's starting byte index.
	 */
	size_t Offset;

	/**
	 * @brief The size of the memory block in bytes.
	 */
	size_t Size;
};

/**
 * @brief Wraps an already allocated buffer to manage it.
 */
class ChunkAllocator
{
public:
	/**
	 * @brief Wraps a buffer.
	 * 
	 * @param data A span to a buffer.
	 */
	ChunkAllocator(size_t size, void* data);

	/**
	 * @brief Allocates memory for a chunk.
	 * 
	 * @param coordinate The coordinate of the chunk.
	 * @param chunk The chunk.
	 *
	 * @return Whether the allocation was successful.
	 */
	auto Allocate(const glm::ivec2& coordinate, const Chunk& chunk) -> bool;

	/**
	 * @brief Frees up the allocated memory of a chunk.
	 * 
	 * @param coordinate The coordinate of the chunk.
	 */
	auto Free(const glm::ivec2& coordinate) -> void;

	/**
	 * @brief Retrieves tzhe mutex of the managed memory.
	 * 
	 * @return The mutex of the managed memory.
	 */
	auto GetMutex() -> std::mutex&
	{
		return m_mutex;
	}

	/**
	 * @brief Retrieves an iterator to the first allocated chunk.
	 * 
	 * @return An iterator to the first allocated chunk.
	 */
	[[nodiscard]] auto begin() const noexcept -> std::unordered_map<glm::ivec2, MemoryBlock>::const_iterator
	{
		return m_allocatedChunks.begin();
	}

	/*
	 * @brief Retrieves an iterator to the last allocated chunk.
	 * 
	 * @return An iterator to the last allocated chunk.
	 */
	[[nodiscard]] auto end() const noexcept -> std::unordered_map<glm::ivec2, MemoryBlock>::const_iterator
	{
		return m_allocatedChunks.end();
	}

private:
	std::span<uint8_t> m_data;
	std::vector<MemoryBlock> m_freeBlocks;
	std::unordered_map<glm::ivec2, MemoryBlock> m_allocatedChunks;
	std::mutex m_mutex;
};
