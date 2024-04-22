#include "Window.h"

#include <GLFW/glfw3.h>

Window::Window(const glm::uvec2& size, std::string_view title)
	: m_size(size)
{
	if(s_windowCount++ == 0u)
	{
		glfwInit();
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_handle = glfwCreateWindow(static_cast<int>(m_size.x), static_cast<int>(m_size.y), title.data(), nullptr, nullptr);
}

Window::~Window()
{
	glfwDestroyWindow(m_handle);

	if(--s_windowCount == 0u)
	{
		glfwTerminate();
	}
}
