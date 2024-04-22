#pragma once

#include <memory>

class Renderer;
struct GLFWwindow;

class Application
{
public:
	Application();
	~Application();

	auto Run() -> void;

private:
	GLFWwindow* m_window;
	std::unique_ptr<Renderer> m_renderer;
};
