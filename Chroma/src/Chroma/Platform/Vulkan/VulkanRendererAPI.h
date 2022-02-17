#pragma once

#include "Chroma/Renderer/RendererAPI.h"

namespace Chroma
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual ~VulkanRendererAPI();

		virtual void Init() override;

		virtual void SetClearColor(const glm::vec4 & color) override;
		virtual void Clear() override;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest) override;

	private:

	};
}