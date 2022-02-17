#include "chromapch.h"
#include "Texture.h"

#include "Chroma/Core/Core.h";
#include "RendererAPI.h"
#include "Chroma/Platform/OpenGL/OpenGLTexture.h"
#include "Chroma/Platform/Vulkan/VulkanTexture.h"

namespace Chroma
{

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height);
			case RendererAPI::API::Vulkan: return CreateRef<VulkanTexture2D>(width, height);
		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path, bool flip)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path, flip);
			case RendererAPI::API::Vulkan: return CreateRef<VulkanTexture2D>(path, flip);
		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

}