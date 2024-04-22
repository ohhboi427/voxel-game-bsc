#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <string_view>

typedef struct GLFWwindow GLFWwindow;

/**
 * @brief A wrapper around GLFWwindow.
 */
class Window
{
public:
	/**
	 * @brief Construct a window.
	 *
	 * Initializes GLFW on the first window construction.
	 * 
	 * @param size The size of the window in pixels.
	 * @param title The title visible on top of the window.
	 */
	Window(const glm::uvec2& size, std::string_view title);

	/**
	 * @brief Deletes thw window.
	 *
	 * Terminates GLFW on the last destruction.
	 */
	~Window();

	Window(const Window&) = delete;
	auto operator=(const Window&) -> Window& = delete;

	Window(Window&&) noexcept = delete;
	auto operator=(Window&&) noexcept -> Window& = delete;

	/**
	 * @brief Retrieves the GLFWwindow handle of the window.
	 */
	[[nodiscard]] explicit constexpr operator GLFWwindow* () const noexcept
	{
		return m_handle;
	}

	/**
	 * @brief Retirves the size of the window.
	 * 
	 * @return The size of window in pixels.
	 */
	[[nodiscard]] constexpr auto GetSize() const noexcept -> const glm::uvec2&
	{
		return m_size;
	}

private:
	inline static uint8_t s_windowCount = 0u;

	GLFWwindow* m_handle;
	glm::uvec2 m_size;
};
