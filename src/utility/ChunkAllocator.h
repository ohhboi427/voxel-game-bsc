#pragma once

#include <span>
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
	ChunkAllocator(std::span<uint8_t> data);

	/**
	 * @brief Allocates a block of a buffer.
	 * 
	 * @param data A span to the allocated memory.
	 * 
	 * @return An object containing the data of the block.
	 */
	[[nodiscard]] auto Allocate(std::span<const uint8_t> data) -> MemoryBlock;

	/**
	 * @brief Deallocates a block of memory.
	 * 
	 * @param chunkBlock An object containing the data of the block.
	 */
	auto Free(const MemoryBlock& chunkBlock) -> void;

private:
	std::span<uint8_t> m_data;
	std::vector<MemoryBlock> m_freeBlocks;
};
