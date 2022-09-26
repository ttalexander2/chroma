#pragma once

#include "Chroma/Renderer/UniformBuffer.h"

namespace Chroma
{
	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t size, uint32_t binding);
		~VulkanUniformBuffer() override;

		void SetData(const void *data, uint32_t size, uint32_t offset = 0) override;

	private:
	};
}
