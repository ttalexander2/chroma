#pragma once

#include "Chroma/Renderer/Pipeline.h"

namespace Chroma
{
	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineSpecification &spec);
		~VulkanPipeline() override;

		PipelineSpecification &GetSpecification() override { return m_Spec; }
		const PipelineSpecification &GetSpecification() const override { return m_Spec; }

		void Initialize() override;

		void Bind() override;


	private:
		PipelineSpecification m_Spec;
	};
}
