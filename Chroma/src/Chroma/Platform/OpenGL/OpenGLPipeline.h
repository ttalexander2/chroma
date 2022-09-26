#pragma once


#include "Chroma/Renderer/Pipeline.h"

namespace Chroma
{
	class OpenGLPipeline : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification &spec);
		~OpenGLPipeline() override;

		PipelineSpecification &GetSpecification() override { return m_Specification; }
		const PipelineSpecification &GetSpecification() const override { return m_Specification; }

		void Initialize() override;
		void Bind() override;

	private:
		PipelineSpecification m_Specification;
		uint32_t m_VertexArrayRendererID = 0;
	};
}
