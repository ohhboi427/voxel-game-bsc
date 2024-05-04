#include "ChunkAllocator.h"

#include <algorithm>
#include <ranges>

ChunkAllocator::ChunkAllocator(std::span<uint8_t> data)
	: m_data(data)
{
	m_freeBlocks.emplace_back(
		MemoryBlock{
			.Offset = 0u,
			.Size = m_data.size(),
		});
}

auto ChunkAllocator::Allocate(const glm::ivec2& coordinate, const Chunk& chunk) -> void
{
	std::scoped_lock lock(m_mutex);

	if(m_allocatedChunks.contains(coordinate))
	{
		return;
	}

	std::span<const uint8_t> data = chunk.Data();

	// Find a free block that is large enough.
	auto it = std::ranges::find_if(
		m_freeBlocks,
		[&] (const MemoryBlock& block) -> bool
		{
			return block.Size >= data.size();
		});

	if(it == m_freeBlocks.end())
	{
		return;
	}

	MemoryBlock chunkBlock{
		.Offset = it->Offset,
		.Size = data.size(),
	};

	// If the free block is the same size as the needed memory, remove the freeblock.
	if(it->Size == data.size())
	{
		m_freeBlocks.erase(it);
	}
	// Otherwise shrink it.
	else
	{
		it->Offset += data.size();
		it->Size -= data.size();
	}

	// Copy the memory into the buffer.
	std::ranges::copy(
		data,
		m_data.subspan(chunkBlock.Offset, chunkBlock.Size).begin()
	);

	m_allocatedChunks.insert({ coordinate, chunkBlock });
}

auto ChunkAllocator::Free(const glm::ivec2& coordinate) -> void
{
	std::scoped_lock lock(m_mutex);

	if(!m_allocatedChunks.contains(coordinate))
	{
		return;
	}

	const MemoryBlock& chunkBlock = m_allocatedChunks.at(coordinate);

	std::vector<MemoryBlock>::iterator itBefore = std::ranges::find_if(
		m_freeBlocks,
		[&] (const MemoryBlock& block) -> bool
		{
			return (block.Offset + block.Size) == chunkBlock.Offset;
		});

	std::vector<MemoryBlock>::iterator itAfter = std::ranges::find_if(
		m_freeBlocks,
		[&] (const MemoryBlock& block) -> bool
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

	m_allocatedChunks.erase(coordinate);
}
