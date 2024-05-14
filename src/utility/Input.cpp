#include "Input.h"

#include "../renderer/Window.h"

#include <GLFW/glfw3.h>

namespace
{
	GLFWwindow* WindowHandle;
}

auto Input::Initialize(const Window& window) -> void
{
	WindowHandle = static_cast<GLFWwindow*>(window);

	glfwSetKeyCallback(
		WindowHandle,
		[] (GLFWwindow* handle, int key, int scancode, int action, int mods) -> void
		{
			if(action == GLFW_REPEAT)
			{
				return;
			}

			Input::OnKey(key, action == GLFW_PRESS);
		});

	glfwSetMouseButtonCallback(
		WindowHandle,
		[] (GLFWwindow* handle, int button, int action, int mods) -> void
		{
			if(action == GLFW_REPEAT)
			{
				return;
			}

			Input::OnMouseButton(button, action == GLFW_PRESS);
		});

	glfwSetCursorPosCallback(
		WindowHandle,
		[] (GLFWwindow* handle, double x, double y) -> void
		{
			glm::vec2 current(static_cast<float>(x), static_cast<float>(y));
			static glm::vec2 last = current;

			glm::vec2 delta = current - last;

			Input::OnCursorMove(current, delta);
			last = current;
		});
}

auto Input::Poll() -> void
{
	glfwPollEvents();
}
