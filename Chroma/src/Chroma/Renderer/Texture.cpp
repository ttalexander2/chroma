#include "chromapch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Chroma/Platform/OpenGL/OpenGLTexture.h"

namespace Chroma
{

	Ref<Texture2D> Chroma::Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path);

		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

}