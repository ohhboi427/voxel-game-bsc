#pragma once

#include "../utility/ChunkAllocator.h"
#include "../world/Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <memory>
#include <unordered_map>

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
	static constexpr size_t DrawDataLocation = 0u;

	const Window& m_window;
	std::unique_ptr<Buffer> m_projectionPropertiesBuffer;
	std::unique_ptr<Buffer> m_screenPropertiesBuffer;
	std::unique_ptr<Buffer> m_chunkDataBuffer;
	std::unique_ptr<Shader> m_screenShader;
	std::unique_ptr<Shader> m_raygenShader;
	std::unique_ptr<ChunkAllocator> m_chunkAllocator;
	std::unordered_map<glm::uvec2, MemoryBlock> m_submittedChunks;
	uint32_t m_renderTexture;
	uint32_t m_dummyVertexArray;

	auto UploadProjectionData() -> void;

	auto DrawChunk(const glm::uvec2& coordinate, const MemoryBlock& block) -> void;
	auto GetChunkLod(const glm::uvec2& coordinate) const noexcept -> uint32_t;
};
