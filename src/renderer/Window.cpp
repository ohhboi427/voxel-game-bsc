#include "Window.h"

#include "../utility/Config.h"

#include <GLFW/glfw3.h>

Window::Window(const WindowSettings& settings)
	: m_settings(settings)
{
	if(s_windowCount++ == 0u)
	{
		glfwInit();
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_handle = glfwCreateWindow(static_cast<int>(m_settings.Size.x), static_cast<int>(m_settings.Size.y), m_settings.Title.data(), nullptr, nullptr);
}

Window::~Window()
{
	glfwDestroyWindow(m_handle);

	if(--s_windowCount == 0u)
	{
		glfwTerminate();
	}
}

auto Window::GetShouldClose() const noexcept -> bool
{
	return static_cast<bool>(glfwWindowShouldClose(m_handle));
}

auto WindowSettings::LoadFromConfig() -> WindowSettings
{
	return WindowSettings{
		.Size = glm::ivec2(
			static_cast<uint32_t>(Config::Get<int64_t>("window", "iWidth")),
			static_cast<uint32_t>(Config::Get<int64_t>("window", "iHeight"))
		),
		.Title = Config::Get<std::string>("window", "sTitle"),
	};
}
