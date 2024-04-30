#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <string>

typedef struct GLFWwindow GLFWwindow;

/**
 * @brief Holds settings related to a window.
 */
struct WindowSettings
{
	/**
	 * @brief The size of the window in pixels.
	 */
	glm::uvec2 Size;

	/**
	 * @brief The title visible on the top of the window.
	 */
	std::string Title;

	/**
	 * @brief Loads the settings from the config file.
	 *
	 * @return The settings loaded from the file.
	 */
	static auto LoadFromConfig() -> WindowSettings;
};

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
	 * @param settings The settings of the window.
	 */
	Window(const WindowSettings& settings);

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
		return m_settings.Size;
	}

private:
	inline static uint8_t s_windowCount = 0u;

	GLFWwindow* m_handle;
	WindowSettings m_settings;
};
