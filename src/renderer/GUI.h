#pragma once

#include "../utility/Action.h"

#include <glm/glm.hpp>

class Window;

namespace GUI
{
	/**
	 * @brief Whether the GUI is visible.
	 */
	inline bool IsVisible = true;

	/**
	 * @brief Called every frame if the GUI is visible.
	 */
	inline Action<glm::uvec2> OnGui;

	/**
	 * @brief Initializes ImGui.
	 * 
	 * @param window The input handler and render target window.
	 */
	auto Initialize(const Window& window) -> void;

	/**
	 * @brief Detrsoys the ImGui context.
	 */
	auto Destroy() -> void;
}
