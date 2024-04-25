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

auto ChunkAllocator::Allocate(const void* data, size_t size) -> MemoryBlock
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
		std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(data), size),
		m_data.subspan(chunkBlock.Offset, chunkBlock.Size).begin()
	);

	return chunkBlock;
}

auto ChunkAllocator::Free(const MemoryBlock& chunkBlock) -> void
{
	std::vector<MemoryBlock>::iterator itBefore = std::ranges::find_if(
		m_freeBlocks,
		[&chunkBlock] (const MemoryBlock& block) -> bool
		{
			return (block.Offset + block.Size) == chunkBlock.Offset;
		});

	std::vector<MemoryBlock>::iterator itAfter = std::ranges::find_if(
		m_freeBlocks,
		[&chunkBlock] (const MemoryBlock& block) -> bool
		{
			return block.Offset == (chunkBlock.Offset + chunkBlock.Size);
		});

	// If there is no adjacent blocks, create a new one.
	if(itBefore == m_freeBlocks.end() && itAfter == m_freeBlocks.end())
	{
		m_freeBlocks.emplace_back(
			MemoryBlock{
				.Offset = chunkBlock.Offset,
				.Size = chunkBlock.Size,
			});
	}
	// If both exists merge them.
	else if(itBefore != m_freeBlocks.end() && itAfter != m_freeBlocks.end())
	{
		itBefore->Size += chunkBlock.Size + itAfter->Size;

		m_freeBlocks.erase(itAfter);
	}
	// If only the one before it exists expand that.
	else if(itBefore != m_freeBlocks.end())
	{
		itBefore->Size += chunkBlock.Size;
	}
	// If only the one after it exists expand that.
	else
	{
		itAfter->Offset -= chunkBlock.Size;
		itAfter->Size += chunkBlock.Size;
	}
}
