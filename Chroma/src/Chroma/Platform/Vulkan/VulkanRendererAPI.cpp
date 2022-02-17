#include "chromapch.h"
#include "VulkanRendererAPI.h"

#include <vulkan/vulkan.h>
#include "Chroma/Platform/Vulkan/VulkanContext.h"


Chroma::VulkanRendererAPI::~VulkanRendererAPI()
{
}

void Chroma::VulkanRendererAPI::Init()
{
}

void Chroma::VulkanRendererAPI::SetClearColor(const glm::vec4& color)
{
}

void Chroma::VulkanRendererAPI::Clear()
{
	VkCommandBuffer commandBuffer = VulkanContext::GetCommandBuffers()[VulkanContext::GetFrameIndex()];
}

void Chroma::VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
}

void Chroma::VulkanRendererAPI::DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest)
{
	VkCommandBuffer commandBuffer = VulkanContext::GetCommandBuffers()[VulkanContext::GetFrameIndex()];
	VkDeviceSize offsets[1] = { 0 };
	vkCmdDrawIndexed(commandBuffer, count, 1, 0, 0, 0);
}
