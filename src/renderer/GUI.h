#pragma once

#include "../utility/Action.h"

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
	inline Action<> OnGui;

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
