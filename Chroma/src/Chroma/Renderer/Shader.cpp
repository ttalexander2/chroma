#include "chromapch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Chroma/Platform/OpenGL/OpenGLShader.h"
#include "Chroma/Platform/Vulkan/VulkanShader.h"




// Adapted from https://www.khronos.org/opengl/wiki/Shader_Compilation

namespace Chroma
{
	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(filePath);
			case RendererAPI::API::Vulkan: return std::make_shared<VulkanShader>(filePath);

		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
			case RendererAPI::API::Vulkan: return std::make_shared<VulkanShader>(name, vertexSrc, fragmentSrc);

		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader, const std::string& name)
	{
		CHROMA_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(shader, name);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		Ref<Shader> shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath, const std::string& name)
	{
		Ref<Shader> shader = Shader::Create(filepath);
		Add(shader, name);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		CHROMA_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}