#include "ChunkAllocator.h"

#include <algorithm>
#include <ranges>

ChunkAllocator::ChunkAllocator(void* data, size_t size)
	: m_data(reinterpret_cast<uint8_t*>(data), size)
{
	m_freeBlocks.emplace_back(
		MemoryBlock{
			.Offset = 0u,
			.Size = size,
		});
}

auto ChunkAllocator::Allocate(void* data, size_t size) -> MemoryBlock
{
	// Find a free block that is large enough.
	auto it = std::ranges::find_if(
		m_freeBlocks,
		[&size] (const MemoryBlock& block) -> bool
		{
			return block.Size >= size;
		});

	if(it == m_freeBlocks.end())
	{
		return {};
	}

	MemoryBlock chunkBlock{
		.Offset = it->Offset,
		.Size = size,
	};

	// If the free block is the same size as the needed memory, remove the freeblock.
	if(it->Size == size)
	{
		m_freeBlocks.erase(it);
	}
	// Otherwise shrink it.
	else
	{
		it->Offset += size;
		it->Size -= size;
	}

	// Copy the memory into the buffer.
	std::ranges::copy(
		std::span<uint8_t>(reinterpret_cast<uint8_t*>(data), size),
		m_data.subspan(chunkBlock.Offset, chunkBlock.Size).begin()
	);

	return chunkBlock;
}

auto ChunkAllocator::Free(const MemoryBlock& chunkBlock) -> void
{
	auto it = std::ranges::find_if(
		m_freeBlocks,
		[&chunkBlock] (const MemoryBlock& block) -> bool
		{
			return
				// If a free block starts right after the freed one.
				(block.Offset == (chunkBlock.Offset + chunkBlock.Size)) ||
				// If a free block ends right before the freed one.
				((block.Offset + block.Size) == chunkBlock.Offset);
		});

	if(it == m_freeBlocks.end())
	{
		m_freeBlocks.emplace_back(
			MemoryBlock{
				.Offset = chunkBlock.Offset,
				.Size = chunkBlock.Size,
			});

		return;
	}

	// Move free block start back if it is after the freed one.
	if(it->Offset > chunkBlock.Offset)
	{
		it->Offset -= chunkBlock.Size;
	}

	it->Size += chunkBlock.Size;
}
