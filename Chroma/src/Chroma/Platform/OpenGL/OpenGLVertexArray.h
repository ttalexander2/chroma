#pragma once

#include <memory>
#include "Chroma/Renderer/VertexArray.h"
#include "Chroma/Renderer/Buffer.h"

namespace Chroma
{
	/// @brief OpenGL Vertex Array
	class OpenGLVertexArray : public VertexArray
	{
	public:
		/// @brief Constructs a new Vertex Array
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray() {}

		// Inherited via VertexArray

		/// @brief Binds a vertex array.
		virtual void Bind() const override;
		/// @brief Unbinds a vertex array.
		virtual void Unbind() const override;
		/// @brief Adds a vertex buffer to this vertex array.
		/// @param vertexBuffer Buffer to add.
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;

		/// @brief Sets the index buffer for this vertex array.
		/// @param indexBuffer to set.
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		/// @brief Get the list of vertex buffers.
		/// @return Array of vertex buffers.
		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }

		/// @brief Get the index buffer.
		/// @return index buffer.
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
	private:
		/// @brief List of vertex buffers.
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;

		/// @brief Index buffer.
		Ref<IndexBuffer> m_IndexBuffer;

		/// @brief Renderer ID.
		uint32_t m_RendererID;

		uint32_t m_VertexBufferIndex = 0;
	};
}
