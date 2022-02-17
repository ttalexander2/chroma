#pragma once

#include "Chroma/Renderer/IndexBuffer.h"

namespace Chroma
{
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