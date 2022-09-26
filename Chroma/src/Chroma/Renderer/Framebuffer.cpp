#include "chromapch.h"
#include "Framebuffer.h"

#include "Chroma/Platform/OpenGL/OpenGLFramebuffer.h"
#include "Chroma/Platform/Vulkan/VulkanFramebuffer.h"
#include "Chroma/Renderer/RendererAPI.h"

namespace Chroma
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RenderAPI::None is currently not supported!");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return CreateRef<OpenGLFramebuffer>(spec);
			case RendererAPI::API::Vulkan:
				return CreateRef<VulkanFramebuffer>(spec);
		}

		CHROMA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
