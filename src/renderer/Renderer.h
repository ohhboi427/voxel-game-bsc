#pragma once

#include "../world/Chunk.h"

#include <glm/glm.hpp>

#include <memory>

class Buffer;
class Shader;

struct GLFWwindow;

class Renderer
{
public:
	Renderer(const glm::uvec2& screenSize, GLFWwindow* window);
	~Renderer();

	auto SubmitChunk(const glm::uvec2& coordinate, const Chunk& chunk) -> void;
	auto RemoveChunk(const glm::uvec2& coordinate) -> void;

	auto Render() -> void;

private:
	GLFWwindow* m_window;
	std::unique_ptr<Buffer> m_projectionPropertiesBuffer;
	std::unique_ptr<Buffer> m_screenPropertiesBuffer;
	std::unique_ptr<Buffer> m_chunkDataBuffer;
	std::unique_ptr<Shader> m_screenShader;
	std::unique_ptr<Shader> m_raygenShader;
	uint32_t m_renderTexture;
	uint32_t m_dummyVertexArray;
};
