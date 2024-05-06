#pragma once

#include "../world/Chunk.h"
#include "../utility/ChunkAllocator.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>

class Buffer;
class Shader;
class RenderTexture;
class Window;
struct Camera;

/**
 * @brief Holds settings related to a window.
 */
struct RendererSettings
{
	/**
	 * @brief The size of the chunk data buffer in bytes.
	 */
	size_t ChunkDataBufferSize;

	/**
	 * @brief Loads the settings from the config file.
	 *
	 * @return The settings loaded from the file.
	 */
	static auto LoadFromConfig() -> RendererSettings;
};

/**
 * @brief Handles the rendering of the world.
 */
class Renderer
{
public:
	/**
	 * @brief Initialized the OpenGL context and the rendering pipeline.
	 *
	 * @param settings The settings of the renderer.
	 * @param window The output window.
	 */
	Renderer(const RendererSettings& settings, const Window& window);
	~Renderer();

	Renderer(const Renderer&) = delete;
	auto operator=(const Renderer&) -> Renderer& = delete;

	Renderer(Renderer&&) noexcept = delete;
	auto operator=(Renderer&&) noexcept -> Renderer& = delete;

	/**
	 * @brief Retrieves the chunk allocator.
	 * 
	 * @return A reference to the chunk allocator.
	 */
	[[nodiscard]] auto GetChunkAllocator() noexcept -> ChunkAllocator&
	{
		return *m_chunkAllocator;
	}

	/**
	 * @brief Updates camera data.
	 *
	 * @param camera The new camera data.
	 */
	auto UpdateProjectionData(const Camera& camera) -> void;

	/**
	 * @brief Renderes the scene.
	 */
	auto Render() -> void;

private:
	static constexpr size_t DrawDataLocation = 0u;

	RendererSettings m_settings;
	const Window& m_window;
	uint32_t m_dummyVertexArray;
	std::unique_ptr<RenderTexture> m_renderTexture;
	std::unique_ptr<Shader> m_screenShader;
	std::unique_ptr<Shader> m_raygenShader;
	std::unique_ptr<Buffer> m_screenPropertiesBuffer;
	std::unique_ptr<Buffer> m_projectionPropertiesBuffer;
	std::unique_ptr<Buffer> m_chunkDataBuffer;
	std::unique_ptr<ChunkAllocator> m_chunkAllocator;

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
	[[nodiscard]] auto GetChunkLod(const glm::ivec2& coordinate) const noexcept -> int32_t;
};
