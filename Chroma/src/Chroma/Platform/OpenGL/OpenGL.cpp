#include "chromapch.h"
#include "OpenGL.h"

GLenum OpenGL::ShaderDataTypeToOpenGLBaseType(Chroma::ShaderDataType type)
{
	switch (type)
	{
	case Chroma::ShaderDataType::Float:    return GL_FLOAT;
	case Chroma::ShaderDataType::Float2:   return GL_FLOAT;
	case Chroma::ShaderDataType::Float3:   return GL_FLOAT;
	case Chroma::ShaderDataType::Float4:   return GL_FLOAT;
	case Chroma::ShaderDataType::Mat3:     return GL_FLOAT;
	case Chroma::ShaderDataType::Mat4:     return GL_FLOAT;
	case Chroma::ShaderDataType::Int:      return GL_INT;
	case Chroma::ShaderDataType::Int2:     return GL_INT;
	case Chroma::ShaderDataType::Int3:     return GL_INT;
	case Chroma::ShaderDataType::Int4:     return GL_INT;
	case Chroma::ShaderDataType::Bool:     return GL_BOOL;

	}
	CHROMA_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}
