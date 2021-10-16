#include "chromapch.h"
#include "Framebuffer.h"

#include "Chroma/Platform/OpenGL/OpenGLFramebuffer.h"
#include "Chroma/Renderer/Renderer.h"

namespace Chroma
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		CHROMA_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return CreateRef<OpenGLFramebuffer>(spec);
		}

		CHROMA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}