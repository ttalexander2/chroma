#include "chromapch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Chroma/Platform/OpenGL/OpenGLBuffer.h"
#include "RendererAPI.h"

namespace Chroma
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices, size);

		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, size);

		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}