#pragma once

#include "Chroma/Core.h"
#include "RenderCommand.h"

namespace Chroma
{
	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	};
}


