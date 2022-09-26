#include "chromapch.h"
#include "OpenGLIndexBuffer.h"

#include "glad/glad.h"

namespace Chroma
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t count) :
		m_Count(count)
	{
		glCreateBuffers(1, &m_RendererId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererId);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
	}
}
