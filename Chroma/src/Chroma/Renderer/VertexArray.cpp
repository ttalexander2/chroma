#include "chromapch.h"
#include "VertexArray.h"
#include "Renderer.h"

#include "Chroma/Platform/OpenGL/OpenGLVertexArray.h"
#include "Chroma/Renderer/RendererAPI.h"

namespace Chroma
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLVertexArray();

		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}