#pragma once

#include <string>
#include "Chroma/Renderer/Shader.h"

// TODO REMOVE
typedef unsigned int GLenum;

namespace Chroma
{
	/// @brief OpenGL Implementation of a shader.
	class OpenGLShader : public Shader
	{
	public:
		/// @brief Construct an OpenGL shader from a file.
		/// @param filePath Path to the GLSL shader.
		OpenGLShader(const std::string& filePath);

		/// @brief Construct an OpenGL shader from a string source.
		/// @param name Name of the shader
		/// @param vertexSrc Vertex shader source.
		/// @param fragmentSrc Fragment shader source.
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		/// Destroys the shader.
		~OpenGLShader();

		/// @brief Binds a shader for use.
		void Bind() const override;

		/// @brief Unbinds a shader.
		void Unbind() const override;

		/// @brief Gets the name of the shader.
		/// @return Shader's name.
		virtual const std::string& GetName() const override { return m_Name; }

		/// @brief Sets a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		virtual void SetUniformInt(const std::string& name, int value) override;

		/// @brief Sets multiple shader uniform values.
		/// @param name Name of the uniform values.
		/// @param value Array of uniform values.
		virtual void SetUniformIntArray(const std::string& name, int* values, int count) override;

		/// @brief Sets a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		virtual void SetUniformFloat(const std::string& name, float value) override;

		/// @brief Sets a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) override;

		/// @brief Sets a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) override;

		/// @brief Sets a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) override;

		/// @brief Sets a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		virtual void SetUniformMat3(const std::string& name, const glm::mat3& value) override;

		/// @brief Sets a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;

		/// @brief Uploads a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		void UploadUniformInt(const std::string& name, int value);

		/// @brief Uploads multiple shader uniform values.
		/// @param name Name of the uniform values.
		/// @param value Array of uniform values.
		void UploadUniformIntArray(const std::string& name, int* values, int count);

		/// @brief Uploads a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		void UploadUniformFloat(const std::string& name, float value);

		/// @brief Uploads a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);

		/// @brief Uploads a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);

		/// @brief Uploads a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		/// @brief Uploads a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		void UploadUniformMat3(const std::string& name, const glm::mat3& value);

		/// @brief Uploads a shader uniform value.
		/// @param name Name of the uniform value.
		/// @param value Uniform value.
		void UploadUniformMat4(const std::string& name, const glm::mat4& value);

	private:
		/// @brief Read shader from file.
		/// @param filePath Path to GLSL shader.
		/// @return Shader source.
		std::string ReadFile(const std::string& filePath);

		/// @brief Preprocess the shader and store the results on the GPU.
		/// @param source Source of the shader.
		/// @return Resulting OpenGL handle for the shader.
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);

		void CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
		void CompileOrGetOpenGLBinaries();
		void CreateProgram();
		void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);

	private:
		uint32_t m_RendererID;
		std::string m_FilePath;
		std::string m_Name;

		std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV;
		std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPRIV;

		std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;
	};
}
