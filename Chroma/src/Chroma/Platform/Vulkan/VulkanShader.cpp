#include "chromapch.h"
#include "VulkanShader.h"

#include "Chroma/Core/Application.h"

#include <fstream>
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <Chroma/Platform/Vulkan/VulkanContext.h>


namespace Chroma
{
	static VkShaderStageFlagBits ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return VK_SHADER_STAGE_VERTEX_BIT;
		if (type == "fragment" || type == "pixel")
			return VK_SHADER_STAGE_FRAGMENT_BIT;

		CHROMA_ASSERT(false, "Unknown shader type '{0}'", type);
		return VK_SHADER_STAGE_VERTEX_BIT;
	}

	static shaderc_shader_kind ShaderStageToShaderC(VkShaderStageFlagBits stage)
	{
		switch (stage)
		{
		case VK_SHADER_STAGE_VERTEX_BIT:   return shaderc_glsl_vertex_shader;
		case VK_SHADER_STAGE_FRAGMENT_BIT: return shaderc_glsl_fragment_shader;
		}
		CHROMA_CORE_ASSERT(false, "Unknown stage!");
		return (shaderc_shader_kind)0;
	}

	static const char* ShaderStageToString(VkShaderStageFlagBits stage)
	{
		switch (stage)
		{
		case VK_SHADER_STAGE_VERTEX_BIT:   return "VK_SHADER_STAGE_VERTEX_BIT";
		case VK_SHADER_STAGE_FRAGMENT_BIT: return "VK_SHADER_STAGE_FRAGMENT_BIT";
		}
		CHROMA_CORE_ASSERT(false, "Unknown stage!");
		return nullptr;
	}

	static const char* ShaderStageCachedVulkanFileExtension(VkShaderStageFlagBits stage)
	{
		switch (stage)
		{
		case VK_SHADER_STAGE_VERTEX_BIT:    return ".cached_vulkan.vert";
		case VK_SHADER_STAGE_FRAGMENT_BIT:  return ".cached_vulkan.frag";
		}
		CHROMA_CORE_ASSERT(false, "Unknown stage!");
		return "";
	}
	

	static const char* GetCacheDirectory()
	{
		// TODO: make sure the assets directory is valid
		return "assets/cache/shader";
	}

	static void CreateCacheDirectoryIfNeeded()
	{
		std::string cacheDirectory = GetCacheDirectory();
		if (!std::filesystem::exists(cacheDirectory))
			std::filesystem::create_directories(cacheDirectory);
	}



	VulkanShader::VulkanShader(const std::string& filePath)
		:m_FilePath(filePath), m_Name(filePath)
	{
		CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(filePath);
		auto shaderSources = PreProcess(source);

		CompileOrGetVulkanBinaries(shaderSources);
		CreateShaderModules();
	}

	VulkanShader::VulkanShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
		:m_Name(name)
	{
		std::unordered_map<VkShaderStageFlagBits, std::string> sources;
		sources[VK_SHADER_STAGE_VERTEX_BIT] = vertexSource;
		sources[VK_SHADER_STAGE_FRAGMENT_BIT] = fragmentSource;

		CompileOrGetVulkanBinaries(sources);
		CreateShaderModules();
	}

	VulkanShader::~VulkanShader()
	{
		for (auto& [type, shader] : m_ShaderModules)
		{
			vkDestroyShaderModule(VulkanContext::GetDevice(), shader, nullptr);
		}
		
	}

	void VulkanShader::Bind() const
	{
	}

	void VulkanShader::Unbind() const
	{
	}

	void VulkanShader::SetUniformInt(const std::string& name, int value)
	{
	}

	void VulkanShader::SetUniformIntArray(const std::string& name, int* values, int count)
	{
	}

	void VulkanShader::SetUniformMat3(const std::string& name, const glm::mat3& value)
	{
	}

	void VulkanShader::SetUniformMat4(const std::string& name, const glm::mat4& value)
	{
	}

	void VulkanShader::SetUniformFloat(const std::string& name, float value)
	{
	}

	void VulkanShader::SetUniformFloat2(const std::string& name, const glm::vec2& value)
	{
	}

	void VulkanShader::SetUniformFloat3(const std::string& name, const glm::vec3& value)
	{
	}

	void VulkanShader::SetUniformFloat4(const std::string& name, const glm::vec4& value)
	{
	}

	std::string VulkanShader::GetName() const
	{
		return std::string();
	}
	std::string VulkanShader::ReadFile(const std::string& filePath)
	{
		std::string result;
		std::ifstream in(filePath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				CHROMA_CORE_ERROR("Could not read from file '{0}'", filePath);
			}
			in.close();
		}
		else
		{
			CHROMA_CORE_ERROR("Could not open file '{0}'", filePath);
		}

		return result;
	}
	std::unordered_map<VkShaderStageFlagBits, std::string> VulkanShader::PreProcess(const std::string& source)
	{
		std::unordered_map<VkShaderStageFlagBits, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			CHROMA_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			//CHROMA_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			VkShaderStageFlagBits glType = ShaderTypeFromString(type);
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);

			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}
	void VulkanShader::CompileOrGetVulkanBinaries(const std::unordered_map<VkShaderStageFlagBits, std::string>& shaderSources)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		options.SetTargetSpirv(shaderc_spirv_version_1_5);

		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = GetCacheDirectory();

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();

		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + ShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, ShaderStageToShaderC(stage), m_FilePath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					CHROMA_CORE_ERROR(module.GetErrorMessage());
					CHROMA_CORE_ASSERT(false, "SPIRV Compilation from Vulkan glsl failed for '{}'!", m_Name);
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}
	void VulkanShader::CreateShaderModules()
	{

		for (auto& [type, data] : m_VulkanSPIRV)
		{
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = data.size() * sizeof(uint32_t);
			createInfo.pCode = reinterpret_cast<const uint32_t*>(data.data());
			
			VkShaderModule shaderModule;
			if (vkCreateShaderModule(VulkanContext::GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create shader module!");
			}
			m_ShaderModules[type] = shaderModule;

			VkPipelineShaderStageCreateInfo shaderStageCreateInfo{};
			shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageCreateInfo.stage = type;
			shaderStageCreateInfo.module = shaderModule;
			shaderStageCreateInfo.pName = "main";

			m_ShaderStageCreateInfo.push_back(shaderStageCreateInfo);
		}
	}
}


