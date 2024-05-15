#include "GUI.h"

#include "Window.h"
#include "../utility/Input.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

auto GUI::Initialize(const Window& window) -> void
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window), true);
	ImGui_ImplOpenGL3_Init("#version 130");

	Input::OnKey += [&] (int key, bool isPressed)
		{
			if(key == GLFW_KEY_ESCAPE && isPressed)
			{
				IsVisible = !IsVisible;

				glfwSetInputMode(static_cast<GLFWwindow*>(window), GLFW_CURSOR, IsVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
			}
		};
}

auto GUI::Destroy() -> void
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
