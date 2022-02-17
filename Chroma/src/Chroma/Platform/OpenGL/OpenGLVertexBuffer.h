#pragma once

#include "Chroma/Renderer/VertexBuffer.h"

namespace Chroma
{
	/// @brief OpenGL Vertex Buffer Object.
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		/// @brief Construct a vertex buffer given a size.
		/// @param size Size of the buffer.
		OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage);

		/// @brief Constructs a vertex buffer given vertices and a size.
		/// @param vertices Vertices to copy into the buffer.
		/// @param size Size of the buffer.
		OpenGLVertexBuffer(float* vertices, uint32_t size, VertexBufferUsage usage);

		/// @brief Frees the GPU memory and destroys the buffer object.
		~OpenGLVertexBuffer();

		/// @brief Binds the vertex buffer.
		virtual void Bind() const override;

		/// @brief Sets the data of the vertex buffer.
		/// @param data Data to write to the buffer.
		/// @param size Size of the data.
		virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;

		/// @brief Gets the layout of the vertex buffer.
		/// @return BufferLayout.
		virtual const VertexBufferLayout& GetLayout() const override { return m_Layout; }

		/// @brief Sets the layout of the buffer.
		/// @param layout BufferLayout
		virtual void SetLayout(const VertexBufferLayout& layout) override { m_Layout = layout; }

		virtual unsigned int GetSize() const override { return m_Size; }

	private:
		/// @brief VBO Renderer ID
		uint32_t m_RendererId;
		/// @brief Layout of the buffer.
		VertexBufferLayout m_Layout;

		uint32_t m_Size;
	};


}
