#pragma once

#include <cstdint>
#include <filesystem>
#include <unordered_map>

/**
 * @brief OpenGL program wrapper.
 */
class Shader
{
public:
	/**
	 * @brief Shader stage source collection.
	 */
	using Sources = std::unordered_map<uint32_t, std::filesystem::path>;

	/**
	 * @brief Creates a shader from multiple stages.
	 *
	 * Loads and compiles multiple shader stages and links them together.
	 * 
	 * @param sources The paths to the shader stages source files.
	 */
	Shader(const Sources& sources);

	/**
	 * @brief Deletes the program.
	 */
	~Shader();

	[[nodiscard]] explicit constexpr operator uint32_t() const noexcept
	{
		return m_handle;
	}

	Shader(const Shader&) = delete;
	auto operator=(const Shader&) -> Shader& = delete;

	Shader(Shader&&) noexcept = delete;
	auto operator=(Shader&&) noexcept -> Shader& = delete;

	/**
	 * @brief Makes the shader active.
	 */
	void Use() const noexcept;

private:
	uint32_t m_handle;
};
