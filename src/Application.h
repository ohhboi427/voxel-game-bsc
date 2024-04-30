#pragma once

#include <memory>

class Renderer;
class Window;
class World;

class Application
{
public:
	Application();
	~Application();

	Application(const Application&) = delete;
	auto operator=(const Application&) -> Application& = delete;

	Application(Application&&) noexcept = delete;
	auto operator=(Application&&) noexcept -> Application& = delete;

	auto Run() -> void;

private:
	std::unique_ptr<Window> m_window;
	std::unique_ptr<Renderer> m_renderer;
	std::unique_ptr<World> m_world;
};
