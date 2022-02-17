#include "chromapch.h"

#include "IndexBuffer.h"
#include "RendererAPI.h"
#include "Chroma/Platform/OpenGL/OpenGlIndexBuffer.h"
#include "Chroma/Platform/Vulkan/VulkanIndexBuffer.h"

namespace Chroma
{
	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLIndexBuffer>(indices, count);
		case RendererAPI::API::Vulkan: return std::make_shared<VulkanIndexBuffer>(indices, count);
		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}