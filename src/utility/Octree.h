#pragma once

#include "Math.h"

#include <cstdint>
#include <span>
#include <vector>

/**
 * @brief A sparse octree of bytes.
 *
 * @tparam L The number of levels of the tree.
 */
template<size_t L>
class Octree
{
public:
	/**
	 * @brief The edge size of the octree.
	 *
	 * Equals to 2^@ref L
	 */
	static constexpr size_t Size = PowerConstexpr(2u, L);

	/**
	 * @brief Retrieves a value from the octree.
	 *
	 * Will return 0 if the value is not stored in the tree.
	 *
	 * @param x The x position of the coordinate.
	 * @param y The y position of the coordinate.
	 * @param z The z position of the coordinate.
	 *
	 * @return A value at the given coordinate.
	 */
	[[nodiscard]] auto Get(size_t x, size_t y, size_t z) const noexcept -> uint8_t
	{
		if(m_nodes.empty())
		{
			return 0u;
		}

		size_t headIndex = 0u;
		size_t parentIndex = 0u;
		uint8_t childIndexInParent = 8u;

		size_t half = m_half;
		while(half >= 1u)
		{
			uint8_t childIndex =
				(x >= half) |
				((y >= half) << 1u) |
				((z >= half) << 2u);

			x -= (x >= half) * half;
			y -= (y >= half) * half;
			z -= (z >= half) * half;

			uint8_t childMask = 1u << childIndex;

			if(!(m_nodes[headIndex] & childMask))
			{
				return 0u;
			}

			size_t skipCount =
				PopCountByte(m_nodes[parentIndex], childIndexInParent + 1u) + // The number of children on the same layer after the current.
				PopCountRange(&m_nodes[parentIndex] + 1u, &m_nodes[headIndex]) + // The number of nodes between the head and parent.
				PopCountByte(m_nodes[headIndex], 0u, childIndex) + // The number of children of head.
				1u; // The head.

			childIndexInParent = childIndex;

			parentIndex = headIndex;
			headIndex += skipCount;

			half /= 2u;
		}

		return m_nodes[headIndex];
	}

	/**
	 * @brief Sets a value in the octree.
	 *
	 * Can't be set to 0.
	 *
	 * @param x The x position of the coordinate.
	 * @param y The y position of the coordinate.
	 * @param z The z position of the coordinate.
	 * @param value The new value.
	 */
	auto Set(size_t x, size_t y, size_t z, uint8_t value) -> void
	{
		if(value == 0u)
		{
			return;
		}

		if(m_nodes.empty())
		{
			m_nodes.emplace_back(0u);
		}

		size_t headIndex = 0u;
		size_t parentIndex = 0u;
		uint8_t childIndexInParent = 8u;

		size_t half = m_half;
		while(half >= 1u)
		{
			uint8_t childIndex =
				(x >= half) |
				((y >= half) << 1u) |
				((z >= half) << 2u);

			x -= (x >= half) * half;
			y -= (y >= half) * half;
			z -= (z >= half) * half;

			uint8_t childMask = 1u << childIndex;

			uint64_t skipCount =
				PopCountByte(m_nodes[parentIndex], childIndexInParent + 1u) + // The number of children on the same layer after the current.
				PopCountRange(&m_nodes[parentIndex] + 1u, &m_nodes[headIndex]) + // The number of nodes between the head and parent.
				PopCountByte(m_nodes[headIndex], 0u, childIndex) + // The number of children of head.
				1u; // The head.

			childIndexInParent = childIndex;

			parentIndex = headIndex;
			headIndex += skipCount;

			if(!(m_nodes[parentIndex] & childMask))
			{
				m_nodes[parentIndex] |= childMask;

				auto new_node = m_nodes.insert(m_nodes.begin() + headIndex, 0u);
			}

			half /= 2u;
		}

		m_nodes[headIndex] = value;
	}

	/**
	 * @brief Retrieves the internal data.
	 * 
	 * @return A span to the bytes of the data.
	 */
	[[nodiscard]] constexpr auto Data() const noexcept -> std::span<const uint8_t>
	{
		return std::span<const uint8_t>(m_nodes.data(), m_nodes.size());
	}

private:
	static constexpr size_t m_half = Size / 2u;

	std::vector<uint8_t> m_nodes;
};
