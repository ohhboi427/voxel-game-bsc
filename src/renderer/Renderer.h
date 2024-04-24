#pragma once

#include "../world/Chunk.h"

#include <glm/glm.hpp>

#include <memory>

class Buffer;
class Shader;
class Window;
struct Camera;

class Renderer
{
public:
	Renderer(const Window& window);
	~Renderer();

	Renderer(const Renderer&) = delete;
	auto operator=(const Renderer&) -> Renderer& = delete;

	Renderer(Renderer&&) noexcept = delete;
	auto operator=(Renderer&&) noexcept -> Renderer& = delete;

	auto SubmitChunk(const glm::uvec2& coordinate, const Chunk& chunk) -> void;
	auto RemoveChunk(const glm::uvec2& coordinate) -> void;

	auto SetCamera(const Camera& camera) -> void;

	auto Render() -> void;

private:
	const Window& m_window;
	std::unique_ptr<Buffer> m_projectionPropertiesBuffer;
	std::unique_ptr<Buffer> m_screenPropertiesBuffer;
	std::unique_ptr<Buffer> m_chunkDataBuffer;
	std::unique_ptr<Shader> m_screenShader;
	std::unique_ptr<Shader> m_raygenShader;
	uint32_t m_renderTexture;
	uint32_t m_dummyVertexArray;

	auto UploadProjectionData() -> void;
};
