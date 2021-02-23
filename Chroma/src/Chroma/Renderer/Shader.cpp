#include "chromapch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Chroma/Platform/OpenGL/OpenGLShader.h"




// Adapted from https://www.khronos.org/opengl/wiki/Shader_Compilation

namespace Chroma
{
	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);

		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}