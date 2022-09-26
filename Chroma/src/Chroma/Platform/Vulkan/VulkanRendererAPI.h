#pragma once

#include "Chroma/Renderer/RendererAPI.h"

namespace Chroma
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		~VulkanRendererAPI() override;

		void Init() override;

		void SetClearColor(const glm::vec4 &color) override;
		void Clear() override;

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest) override;

	private:
	};
}
