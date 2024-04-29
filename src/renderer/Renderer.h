#pragma once

#include "../utility/ChunkAllocator.h"
#include "../world/Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <memory>
#include <mutex>
#include <unordered_map>

class Buffer;
class Shader;
class Window;
struct Camera;

/**
 * @brief Handles the rendering of the world.
 */
class Renderer
{
public:
	/**
	 * @brief Initialized the OpenGL context and the rendering pipeline.
	 *
	 * @param window The output window.
	 */
	Renderer(const Window& window);
	~Renderer();

	Renderer(const Renderer&) = delete;
	auto operator=(const Renderer&)->Renderer & = delete;

	Renderer(Renderer&&) noexcept = delete;
	auto operator=(Renderer&&) noexcept -> Renderer & = delete;

	/**
	 * @brief Submits a chunk to be rendered.
	 *
	 * @param coordinate The coordinate of the chunk.
	 * @param chunk The submitted chunk.
	 */
	auto SubmitChunk(const glm::ivec2& coordinate, const Chunk& chunk) -> void;

	/**
	 * @brief Removes a chunk from the submitted chunks.
	 *
	 * @param coordinate The coordinate of the chunk.
	 */
	auto RemoveChunk(const glm::ivec2& coordinate) -> void;

	/**
	 * @brief Updates camera data.
	 *
	 * @param camera The new camera data.
	 */
	auto SetCamera(const Camera& camera) -> void;

	/**
	 * @brief Renderes the scene.
	 */
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
	std::mutex m_chunkDataMutex{};
	std::unordered_map<glm::ivec2, MemoryBlock> m_submittedChunks;
	uint32_t m_dummyVertexArray;
	uint32_t m_renderTexture;

	/**
	 * @brief Initializes the chunk data buffer and allocator.
	 */
	auto InitializeChunkData() -> void;

	/**
	 * @brief Initialies the projection and screen data buffers.
	 */
	auto InitializeProjectionData() -> void;

	/**
	 * @brief Issues a draw call to draw one chunk.
	 *
	 * @param coordinate The coordinate of the chunk.
	 * @param block The memory block of the chunk.
	 */
	auto DrawChunk(const glm::ivec2& coordinate, const MemoryBlock& block) -> void;

	/**
	 * @brief Calculate the LOD of a chunk.
	 *
	 * @param coordinate The coordinate of the chunk.
	 */
	auto GetChunkLod(const glm::ivec2& coordinate) const noexcept -> int32_t;
};
