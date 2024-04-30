#pragma once

#include <cstdint>
#include <span>

/**
 * @brief Creates a span out of a pointer.
 * 
 * @tparam T The type of the elements in the span.
 * 
 * @param size The size of the span.
 * @param data A pointer to the elements.
 * 
 * @return A span to the elements.
 */
template<typename T = uint8_t>
[[nodiscard]] constexpr auto Span(size_t size, T* data = nullptr) noexcept -> std::span<T>
{
	return std::span<T>(data, size);
}
