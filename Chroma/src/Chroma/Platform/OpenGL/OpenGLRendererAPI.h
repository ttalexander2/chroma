#pragma once

#include "Chroma/Renderer/RendererAPI.h"

namespace Chroma
{
	/// @brief OpenGL Renderer API. This class executes OpenGL specific rendering API calls.
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		/// @brief Initialize OpenGL Rendering
		void Init() override;

		/// @brief Set the screen clear color.
		/// @param color Color to clear the screen.
		void SetClearColor(const glm::vec4 &color) override;

		/// @brief Clear the screen.
		void Clear() override;

		/// @brief Set the OpenGL viewport.
		/// @param x X position of the viewport.
		/// @param y Y position of the viewport.
		/// @param width Width of the viewport.
		/// @param height Height of the viewport.
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		/// @brief Draw an indexed vertex array.
		/// @param vertexArray Vertex array to draw.
		/// @param indexCount Number of indices to draw.
		void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest) override;
	};
}
