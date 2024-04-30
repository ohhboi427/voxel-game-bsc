#include "Buffer.h"

#include <glad/gl.h>

Buffer::Buffer(std::span<const uint8_t> data, uint32_t flags)
	: m_size(data.size()), m_mappingFlags(flags & ~GL_DYNAMIC_STORAGE_BIT), m_mappedStorage(nullptr)
{
	glCreateBuffers(1, &m_handle);
	glNamedBufferStorage(
		m_handle,
		static_cast<GLsizeiptr>(m_size),
		data.data(),
		static_cast<GLbitfield>(flags) & ~GL_MAP_FLUSH_EXPLICIT_BIT
	);

	if(flags & GL_MAP_PERSISTENT_BIT)
	{
		Map();
	}
}

Buffer::~Buffer()
{
	if(m_mappedStorage != nullptr)
	{
		Unmap();
	}

	glDeleteBuffers(1, &m_handle);
}

auto Buffer::SetData(size_t offset, std::span<const uint8_t> data) noexcept -> void
{
	glNamedBufferSubData(m_handle, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(data.size()), data.data());
}

auto Buffer::Map(uint32_t flags) noexcept -> void*
{
	if(m_mappedStorage != nullptr)
	{
		return m_mappedStorage;
	}

	m_mappedStorage = glMapNamedBufferRange(
		m_handle,
		0, static_cast<GLsizeiptr>(m_size),
		static_cast<GLbitfield>((flags != 0u) ? (flags & m_mappingFlags) : m_mappingFlags)
	);

	return m_mappedStorage;
}

auto Buffer::Unmap() noexcept -> void
{
	glUnmapNamedBuffer(m_handle);

	m_mappedStorage = nullptr;
}

auto Buffer::Flush(size_t offset, size_t size) noexcept -> void
{
	glFlushMappedNamedBufferRange(m_handle, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size));
}

auto Buffer::Bind(uint32_t target) const noexcept -> void
{
	glBindBuffer(target, m_handle);
}

auto Buffer::Bind(uint32_t target, uint32_t index) const noexcept -> void
{
	glBindBufferBase(target, index, m_handle);
}
