#include "chromapch.h"
#include "VertexBuffer.h"

#include "Chroma/Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Chroma/Platform/Vulkan/VulkanVertexBuffer.h"
#include "RendererAPI.h"

namespace Chroma
{
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, VertexBufferUsage usage)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(size, usage);
		case RendererAPI::API::Vulkan: return std::make_shared<VulkanVertexBuffer>(size, usage);

		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size, VertexBufferUsage usage)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(vertices, size, usage);
			case RendererAPI::API::Vulkan: return std::make_shared<VulkanVertexBuffer>(vertices, size, usage);

		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

}