#pragma once

#include "Chroma/Renderer/VertexBuffer.h"
#include "vulkan/vulkan.h"

namespace Chroma
{


	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(uint32_t size, VertexBufferUsage usage);
		VulkanVertexBuffer(float* vertices, uint32_t size, VertexBufferUsage usage);
		~VulkanVertexBuffer();

		virtual void Bind() const override;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;

		virtual const VertexBufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const VertexBufferLayout& layout) override { m_Layout = layout; }
		virtual uint32_t GetSize() const override { return m_Size; }

		//VULKAN SPECIFIC
		VkVertexInputBindingDescription GetBindingDescription();
		std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

	private:
		void CreateBuffer();

	private:
		VertexBufferLayout m_Layout;
		uint32_t m_Size;
		VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_VertexMemory = VK_NULL_HANDLE;
	};


}