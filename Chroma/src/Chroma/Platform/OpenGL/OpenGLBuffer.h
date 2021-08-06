#pragma once

#include "Chroma/Renderer/Buffer.h"

namespace Chroma
{
	/// @brief OpenGL Vertex Buffer Object.
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		/// @brief Construct a vertex buffer given a size.
		/// @param size Size of the buffer.
		OpenGLVertexBuffer(uint32_t size);

		/// @brief Constructs a vertex buffer given vertices and a size.
		/// @param vertices Vertices to copy into the buffer.
		/// @param size Size of the buffer.
		OpenGLVertexBuffer(float* vertices, uint32_t size);

		/// @brief Frees the GPU memory and destroys the buffer object.
		~OpenGLVertexBuffer();

		/// @brief Binds the vertex buffer.
		virtual void Bind() const override;

		/// @brief Unbinds the vertex buffer.
		virtual void Unbind() const override;

		/// @brief Sets the data of the vertex buffer.
		/// @param data Data to write to the buffer.
		/// @param size Size of the data.
		virtual void SetData(const void* data, uint32_t size) override;

		/// @brief Gets the layout of the vertex buffer.
		/// @return BufferLayout.
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }

		/// @brief Sets the layout of the buffer.
		/// @param layout BufferLayout
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	private:
		/// @brief VBO Renderer ID
		uint32_t m_RendererId;
		/// @brief Layout of the buffer.
		BufferLayout m_Layout;
	};

	/// @brief OpenGL Index Buffer
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		/// @brief Constructs an OpenGL Index Buffer.
		/// @param indices Indices to write to the buffer.
		/// @param count Number of indices provided.
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);

		/// @brief Destroys the OpenGL Index Buffer and frees memory on the GPU.
		~OpenGLIndexBuffer();

		/// @brief Binds the buffer on the GPU.
		virtual void Bind() const override;

		/// @brief Unbinds the buffer on the GPU.
		virtual void Unbind() const override;

		/// @brief Gets the number of indices.
		/// @return Count.
		virtual uint32_t GetCount() const override { return m_Count; }

	private:
		/// @brief Index Buffer Renderer ID.
		uint32_t m_RendererId;
		/// @brief Number of indices.
		uint32_t m_Count;
	};
}
