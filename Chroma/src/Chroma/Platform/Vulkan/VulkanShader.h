#pragma once

#include <Chroma/Renderer/Shader.h>
#include <vulkan/vulkan.h>

namespace Chroma
{
	class VulkanContext;

	class VulkanShader : public Shader
	{
		friend class VulkanContext;

	public:
		VulkanShader(const std::string &filePath);
		VulkanShader(const std::string &name, const std::string &vertexSource, const std::string &fragmentSource);
		~VulkanShader() override;

		void Bind() const override;
		void Unbind() const override;

		void SetUniformInt(const std::string &name, int value) override;
		void SetUniformIntArray(const std::string &name, int *values, int count) override;

		void SetUniformMat3(const std::string &name, const glm::mat3 &value) override;
		void SetUniformMat4(const std::string &name, const glm::mat4 &value) override;

		void SetUniformFloat(const std::string &name, float value) override;
		void SetUniformFloat2(const std::string &name, const glm::vec2 &value) override;
		void SetUniformFloat3(const std::string &name, const glm::vec3 &value) override;
		void SetUniformFloat4(const std::string &name, const glm::vec4 &value) override;

		std::string GetName() const override;


	private:
		std::string ReadFile(const std::string &filePath);

		std::unordered_map<VkShaderStageFlagBits, std::string> PreProcess(const std::string &source);
		void CompileOrGetVulkanBinaries(const std::unordered_map<VkShaderStageFlagBits, std::string> &shaderSources);

		void CreateShaderModules();

	private:
		std::string m_FilePath;
		std::string m_Name;
		std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>> m_VulkanSPIRV;
		std::unordered_map<VkShaderStageFlagBits, VkShaderModule> m_ShaderModules;
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStageCreateInfo;
	};
}
