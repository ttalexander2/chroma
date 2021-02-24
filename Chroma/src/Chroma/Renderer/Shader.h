#pragma once

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

namespace Chroma
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void UploadUniformInt(const std::string& name, int value) = 0;

		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& value) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filePath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

	};


	class ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader, const std::string& name);

		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& filepath, const std::string& name);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;

	};
}
