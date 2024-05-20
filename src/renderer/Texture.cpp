#include "Texture.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <stb/stb_image.h>

Texture::Texture(const glm::uvec2& size, uint32_t format, uint32_t unit)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_handle);
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage2D(m_handle, 1, format, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
	glBindTextureUnit(unit, m_handle);
	glBindImageTexture(unit, m_handle, 0, GL_FALSE, 0, GL_READ_WRITE, format);
}

Texture::Texture(const std::filesystem::path& path, uint32_t unit)
{
	int32_t width, height, channels;
	uint8_t* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_handle);
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage2D(m_handle, 1, (channels == 4) ? GL_RGBA8 : GL_RGB, width, height);
	glTextureSubImage2D(m_handle, 0, 0, 0, width, height, (channels == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
	glBindTextureUnit(unit, m_handle);

	stbi_image_free(data);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_handle);
}

auto Texture::Clear(const glm::vec4& color) const -> void
{
	glClearTexImage(m_handle, 0u, GL_RGBA, GL_FLOAT, glm::value_ptr(color));
}
