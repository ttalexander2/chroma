#include "chromapch.h"
#include "UniformBuffer.h"

#include "Chroma/Renderer/Renderer.h"
#include "Chroma/Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Chroma
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	CHROMA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		CHROMA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}