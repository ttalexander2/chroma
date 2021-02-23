#pragma once

#include <string>
#include "Chroma/Renderer/Shader.h"

namespace Chroma
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		virtual void UploadUniformInt(const std::string& name, int value) override;

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& value) override;

		void UploadUniformMat3(const std::string& name, const glm::mat3& uniform);
		void UploadUniformMat4(const std::string& name, const glm::mat4& uniform);

	private:
		uint32_t m_RendererID;
	};
}
