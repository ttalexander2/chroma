#include "chromapch.h"
#include "VulkanVertexBuffer.h"

#include "VulkanContext.h"


namespace Chroma
{
	static VkFormat ShaderDataTypeToVulkanDataType(Chroma::ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Int: return VK_FORMAT_R32_SINT;
		case ShaderDataType::Int2: return VK_FORMAT_R32G32_SINT;
		case ShaderDataType::Int3: return VK_FORMAT_R32G32B32_SINT;
		case ShaderDataType::Int4: return VK_FORMAT_R32G32B32A32_SINT;
		case ShaderDataType::Float: return VK_FORMAT_R32_SFLOAT;
		case ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
		case ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
		case ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
		//case ShaderDataType::Mat3: VK_FORMAT_
		default: return VK_FORMAT_UNDEFINED;
		}
	}
	

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size, VertexBufferUsage usage)
		:m_Size(size)
	{
		CreateBuffer();
	}

	VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size, VertexBufferUsage usage)
		: m_Size(size)
	{
		CreateBuffer();
		SetData((void*)vertices, size, 0);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		vkDestroyBuffer(VulkanContext::GetDevice(), m_VertexBuffer, nullptr);
		vkFreeMemory(VulkanContext::GetDevice(), m_VertexMemory, nullptr);
	}

	void VulkanVertexBuffer::Bind() const
	{
		VkCommandBuffer commandBuffer = VulkanContext::GetCommandBuffers()[VulkanContext::GetFrameIndex()];
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer, offsets);
	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		//Create staging buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VulkanContext::CreateBuffer(VulkanContext::GetDevice(), m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);


		//Copy data to staging buffer
		void* buff;
		vkMapMemory(VulkanContext::GetDevice(), stagingBufferMemory, 0, size, 0, &buff);
		memcpy(buff, data, (size_t)size);
		vkUnmapMemory(VulkanContext::GetDevice(), stagingBufferMemory);

		//Copy data from staging buffer to vertex buffer
		VulkanContext::CopyBuffer(stagingBuffer, m_VertexBuffer, size);

		//Destroy staging buffer
		vkDestroyBuffer(VulkanContext::GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(VulkanContext::GetDevice(), stagingBufferMemory, nullptr);
	}

	VkVertexInputBindingDescription VulkanVertexBuffer::GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDesc{};
		bindingDesc.binding = 0;
		bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		bindingDesc.stride = m_Layout.GetStride();

		return bindingDesc;
	}

	std::vector<VkVertexInputAttributeDescription> VulkanVertexBuffer::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> descriptions;
		int i = 0;
		for (auto& bufferElement : m_Layout)
		{
			VkVertexInputAttributeDescription desc{};
			desc.binding = 0;
			desc.location = i;
			desc.offset = bufferElement.Offset;
			desc.format = ShaderDataTypeToVulkanDataType(bufferElement.Type);
			descriptions[i] = desc;
			
			i++;
		}

		return descriptions;
	}

	void VulkanVertexBuffer::CreateBuffer()
	{
		VulkanContext::CreateBuffer(VulkanContext::GetDevice(), m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexMemory);
	}
}