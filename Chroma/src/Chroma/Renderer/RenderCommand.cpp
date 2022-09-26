#include "chromapch.h"
#include "RenderCommand.h"

#include "Chroma/Platform/OpenGL/OpenGLRendererAPI.h"
#include "Chroma/Platform/Vulkan/VulkanRendererAPI.h"

namespace Chroma
{
	RendererAPI *RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}
