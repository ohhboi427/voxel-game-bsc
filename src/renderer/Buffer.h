#pragma once

#include <cstdint>
#include <span>

/**
 * @brief OpenGL buffer wrapper.
 */
class Buffer
{
public:
	/**
	 * @brief Constructs an OpenGL buffer.
	 *
	 * It will map the buffer if the 'GL_MAP_PERSISTENT_BIT' is set.
	 *
	 * @param data A span to the initial data.
	 * @param flags Flags of the buffer's data storage.
	 */
	Buffer(std::span<const uint8_t> data = {}, uint32_t flags = 0u);

	/**
	 * @brief Deletes the buffer.
	 *
	 * It will also unmap the buffer if it wasn't unmapped explicitly before deletion.
	 */
	~Buffer();

	Buffer(const Buffer&) = delete;
	auto operator=(const Buffer&) -> Buffer& = delete;

	Buffer(Buffer&&) noexcept = delete;
	auto operator=(Buffer&&) noexcept -> Buffer& = delete;

	/**
	 * @brief Retrieves the GLuint handle of the buffer.
	 */
	[[nodiscard]] explicit constexpr operator uint32_t() const noexcept
	{
		return m_handle;
	}

	/**
	 * @brief Retrieves the buffer's mapped storage.
	 *
	 * @tparam The type of the data.
	 *
	 * @return A span to the mapped storage.
	 */
	template<typename T = uint8_t>
	[[nodiscard]] auto GetMappedStorage() const noexcept -> std::span<T>
	{
		return std::span<T>(static_cast<T*>(m_mappedStorage), m_size);
	}

	/**
	 * @brief Sets a range of the buffer's data.
	 *
	 * Data can only be changed using this if the 'GL_DYNAMIC_STORAGE_BIT' was set at creation.
	 *
	 * @param offset The offset of the set range.
	 * @param data A span to data copied to the buffer.
	 */
	auto SetData(size_t offset, std::span<const uint8_t> data) noexcept -> void;

	/**
	 * @brief Maps the buffer's storage.
	 *
	 * It will use the only those flags, which were given at creation.
	 * If 'flags' is 0, it will default to the initial flags.
	 *
	 * @param flags Tha mapping flags.
	 *
	 * @return A pointer to the mapped storage.
	 */
	auto Map(uint32_t flags = 0u) noexcept -> void*;

	/**
	 * @brief Unmaps the buffer's storage.
	 */
	auto Unmap() noexcept -> void;

	/**
	 * @brief If the storgae was mapped with 'GL_MAP_FLUSH_EXPLICIT_BIT' set, it will force a flush.
	 *
	 * @param offset The offset of the flushed range.
	 * @param size The size of the flushed range.
	 */
	auto Flush(size_t offset, size_t size) noexcept -> void;

	/**
	 * @brief Binds the buffer to a target.
	 *
	 * @param target The target binding.
	 */
	auto Bind(uint32_t target) const noexcept -> void;

	/**
	 * @brief Binds the buffer to an indexed target.
	 *
	 * @param target The target binding.
	 * @param index The index of the the binding point.
	 */
	auto Bind(uint32_t target, uint32_t index) const noexcept -> void;

private:
	uint32_t m_handle;
	size_t m_size;
	uint32_t m_mappingFlags;
	void* m_mappedStorage;
};

namespace Literals
{
	/**
	 * @brief Forms a literal representing a byte size.
	 * 
	 * @param size The number of bytes.
	 * 
	 * @return The original value.
	 */
	[[nodiscard]] consteval auto operator""_b(uint64_t size) noexcept -> uint64_t
	{
		return size;
	}

	/**
	 * @brief Forms a literal representing a kilobyte size.
	 * 
	 * @param size The number of kilobytes.
	 * 
	 * @return The original value multiplies by 1024.
	 */
	[[nodiscard]] consteval auto operator""_kb(uint64_t size) noexcept -> uint64_t
	{
		return size * 1024u;
	}

	/**
	 * @brief Forms a literal representing a megabyte size.
	 * 
	 * @param size The number of megabytes.
	 * 
	 * @return The original value multiplies by 1048576.
	 */
	[[nodiscard]] consteval auto operator""_mb(uint64_t size) noexcept -> uint64_t
	{
		return size * 1024u * 1024u;
	}
}
