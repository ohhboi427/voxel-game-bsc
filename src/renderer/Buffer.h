#pragma once

#include <type_traits>

#include <cstdint>

/**
 * @brief OpenGL buffer wrapper.
 */
class Buffer
{
public:
	/**
	 * @brief Constructs an OpenGl buffer.
	 *
	 * It will map the buffer if the 'GL_MAP_PERSISTENT_BIT' is set.
	 *
	 * @param size The size of the buffer.
	 * @param data The initial data of the buffer.
	 * @param flags Flags of the buffer's data storage.
	 */
	Buffer(size_t size, const void* data = nullptr, uint32_t flags = 0u);

	/**
	 * @brief Deletes the buffer.
	 *
	 * It will also unmap the buffer if it wasn't unmapped explicitly before deletion.
	 */
	~Buffer();

	Buffer(const Buffer&) = delete;
	auto operator=(const Buffer&)->Buffer & = delete;

	Buffer(Buffer&&) noexcept = delete;
	auto operator=(Buffer&&) noexcept -> Buffer & = delete;

	/**
	 * @brief Retrieves a pointer to the buffer's mapped storage.
	 *
	 * @tparam The type of the pointer.
	 *
	 * @return A pointer to the mapped storage.
	 */
	template<typename T = void>
	[[nodiscard]] auto GetMappedStorage() const noexcept -> T*
	{
		return static_cast<T*>(m_mappedStorage);
	}

	/**
	 * @brief Sets a range of the buffer's data.
	 *
	 * Data can only be changed using this if the 'GL_DYNAMIC_STORAGE_BIT' was set at creation.
	 *
	 * @param offset The offset of the set range.
	 * @param size The size of the set range.
	 * @param data A pointer to data copied to the buffer.
	 */
	auto SetData(size_t offset, size_t size, const void* data) noexcept -> void;

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
