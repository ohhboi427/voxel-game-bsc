#pragma once

#include <bit>
#include <cstdint>

/**
 * @brief Counts the number of set bits in a byte.
 *
 * @param byte The byte.
 * @param offset The offset of the starting bit.
 * @param count The number of counted bits.
 *
 * @return The number of set bits.
 */
[[nodiscard]] constexpr auto PopCountByte(uint8_t byte, uint8_t offset = 0u, uint8_t count = 8u) noexcept -> size_t
{
	uint8_t mask = ~(~0u << count) << offset;
	byte &= mask;

	return std::popcount(byte);
}

/**
 * @brief Counts the set bits in multiple bytes.
 *
 * @param begin A pointer to the first byte.
 * @param end A pointer after the last byte.
 *
 * @return The number of set bits.
 */
[[nodiscard]] constexpr auto PopCountRange(const uint8_t* begin, const uint8_t* end) noexcept -> size_t
{
	uint64_t sum = 0u;

	while(begin < end)
	{
		sum += std::popcount(*begin);

		++begin;
	}

	return sum;
}

/**
 * @brief A constexpr unsigned integer power function.
 *
 * @param base The base of the power.
 * @param exponent The exponent of the power.
 *
 * @return The result of the power.
 */
[[nodiscard]] constexpr auto PowerConstexpr(uint64_t base, uint64_t exponent) noexcept -> uint64_t
{
	uint64_t result = 1u;

	for(uint64_t i = 0u; i < exponent; ++i)
	{
		result *= base;
	}

	return result;
}
