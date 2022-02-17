#include "chromapch.h"
#include "Pipeline.h"

#include "Chroma/Renderer/RendererAPI.h"
#include "Chroma/Platform/OpenGL/OpenGLPipeline.h"
#include "Chroma/Platform/Vulkan/VulkanPipeline.h"




namespace Chroma
{


	Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: CHROMA_CORE_ASSERT(false, "RendererAPI::API::None not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLPipeline>(spec);
		case RendererAPI::API::Vulkan: return std::make_shared<VulkanPipeline>(spec);
		}

		CHROMA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

}


