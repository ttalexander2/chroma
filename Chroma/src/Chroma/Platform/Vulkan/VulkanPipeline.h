#pragma once

#include "Chroma/Renderer/Pipeline.h"

namespace Chroma
{
	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineSpecification& spec);
		virtual ~VulkanPipeline();

		virtual PipelineSpecification& GetSpecification() override { return m_Spec; }
		virtual const PipelineSpecification& GetSpecification() const override { return m_Spec; }

		virtual void Initialize() override;

		virtual void Bind() override;


	private:
		PipelineSpecification m_Spec;
	};
}