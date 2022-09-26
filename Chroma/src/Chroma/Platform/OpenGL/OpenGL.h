#pragma once

#include <glad/glad.h>
#include <Chroma/Renderer/VertexBuffer.h>

namespace OpenGL
{
	GLenum ShaderDataTypeToOpenGLBaseType(Chroma::ShaderDataType type);
}
