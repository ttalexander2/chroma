#pragma once

#include <string>
#include "Chroma/Renderer/Shader.h"

// TODO REMOVE
typedef unsigned int GLenum;

namespace Chroma
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		virtual const std::string& GetName() const override { return m_Name; }

		virtual void UploadUniformInt(const std::string& name, int value) override;

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& value) override;

		void UploadUniformMat3(const std::string& name, const glm::mat3& uniform);
		void UploadUniformMat4(const std::string& name, const glm::mat4& uniform);

	private:
		std::string ReadFile(const std::string& filePath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>&);

	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};
}
