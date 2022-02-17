#include "chromapch.h"
#include "UniformBuffer.h"

#include "Chroma/Renderer/RendererAPI.h"
#include "Chroma/Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Chroma/Platform/Vulkan/VulkanUniformBuffer.h"

namespace Chroma
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	CHROMA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(size, binding);
		case RendererAPI::API::Vulkan: return CreateRef<VulkanUniformBuffer>(size, binding);
		}

		CHROMA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}