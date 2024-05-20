#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <unordered_map>
#include <filesystem>

/**
 * @brief OpenGL texture wrapper.
 */
class Texture
{
public:
	/**
	 * @brief Initializes a texture as a render target.
	 *
	 * It binds the texture as an image.
	 * 
	 * @param size The size of the texture in pixels.
	 * @param format The format used to store the texture's data.
	 * @param unit The unit that texture will be bound to.
	 */
	Texture(const glm::uvec2& size, uint32_t format, uint32_t unit = 0u);

	/**
	 * @brief Loads a texture from a file.
	 * 
	 * @param path The path of the file.
	 */
	Texture(const std::filesystem::path& path, uint32_t unit = 0u);

	/**
	 * @brief Deletes the texture.
	 */
	~Texture();

	/**
	 * @brief Sets all pixels of the texture to a value.
	 * 
	 * @param color The new value of the pixels.
	 */
	auto Clear(const glm::vec4& color) const -> void;

	/**
	 * @brief Retrieves the GLuint handle of the program.
	 */
	[[nodiscard]] explicit constexpr operator uint32_t() const noexcept
	{
		return m_handle;
	}

private:
	uint32_t m_handle;
};
