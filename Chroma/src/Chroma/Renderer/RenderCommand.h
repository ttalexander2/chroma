#pragma once

#include "RendererAPI.h"

namespace Chroma
{
	class RenderCommand
	{
	public:
		static void Init()
		{
			s_RendererAPI->Init();
		}


		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4 &color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		static void Clear()
		{
			s_RendererAPI->Clear();
		}

		static void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest)
		{
			s_RendererAPI->DrawIndexed(count, type, depthTest);
		}

	private:
		static RendererAPI *s_RendererAPI;
	};
}
