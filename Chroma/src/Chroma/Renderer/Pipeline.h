#pragma once

#include "Chroma/Core/Core.h"
#include "Chroma/Renderer/VertexBuffer.h"
#include "Chroma/Renderer/Shader.h"


namespace Chroma
{
	struct PipelineSpecification
	{
		Ref<Shader> Shader;
		VertexBufferLayout Layout;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual PipelineSpecification &GetSpecification() = 0;
		virtual const PipelineSpecification &GetSpecification() const = 0;

		virtual void Initialize() = 0;

		virtual void Bind() = 0;

		static Ref<Pipeline> Create(const PipelineSpecification &spec);
	};
}
