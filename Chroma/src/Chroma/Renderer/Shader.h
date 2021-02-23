#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Chroma
{
	class Shader
	{
	public:;
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void UploadUniformInt(const std::string& name, int value) = 0;

		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& value) = 0;

		static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);

	};
}
