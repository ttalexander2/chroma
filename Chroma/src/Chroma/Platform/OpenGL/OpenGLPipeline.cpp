#include "chromapch.h"
#include "OpenGLPipeline.h"

#include "OpenGL.h"

namespace Chroma
{
	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec)
		: m_Specification(spec)
	{
		Initialize();
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
		glDeleteVertexArrays(1, &m_VertexArrayRendererID);
	}

	void OpenGLPipeline::Initialize()
	{
		if (m_VertexArrayRendererID)
			glDeleteVertexArrays(1, &m_VertexArrayRendererID);

		glGenVertexArrays(1, &m_VertexArrayRendererID);
		glBindVertexArray(m_VertexArrayRendererID);
		Bind();
	}

	void OpenGLPipeline::Bind()
	{
		glBindVertexArray(m_VertexArrayRendererID);

		const auto& layout = m_Specification.Layout;
		uint32_t attribute = 0;
		for (const auto& element : layout)
		{
			auto glBaseType = OpenGL::ShaderDataTypeToOpenGLBaseType(element.Type);
			glEnableVertexAttribArray(attribute);
			if (glBaseType == GL_INT)
			{
				glVertexAttribIPointer(attribute, element.GetComponentCount(), glBaseType, layout.GetStride(), (const void*)(intptr_t)element.Offset);
			}
			else
			{
				glVertexAttribPointer(attribute, element.GetComponentCount(), glBaseType, element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)(intptr_t)element.Offset);
			}
			attribute++;

		}

	}
}


