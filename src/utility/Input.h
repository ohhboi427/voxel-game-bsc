#pragma once

#include "Action.h"

#include <glm/glm.hpp>

#include <cstdint>

class Window;

namespace Input
{
	/**
	 * @brief Called when a key is either pressed or released. The first parameter is the code of the key, the second is whether it was pressed.
	 */
	inline Action<int, bool> OnKey;

	/**
	 * @brief Called when a mouse button is either pressed or released. The first parameter is the code of the button, the second is whether it was pressed.
	 */
	inline Action<int, bool> OnMouseButton;

	/**
	 * @brief Called when the cursor is moved. The first parameter is the current position of the cursor, the second is the delta movement.
	 */
	inline Action<glm::vec2, glm::vec2> OnCursorMove;

	/**
	 * @brief Initialized the input callbacks.
	 * 
	 * @param window The input capturing window.
	 */
	auto Initialize(const Window& window) -> void;
}
