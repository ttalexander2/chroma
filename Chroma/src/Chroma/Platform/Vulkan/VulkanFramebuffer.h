#pragma once


#include "Chroma/Renderer/Framebuffer.h"

namespace Chroma
{

	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		virtual ~VulkanFramebuffer() override;

		void Bind() override;
		void Unbind() override;
		void Invalidate() override;
		void Resize(uint32_t width, uint32_t height) override;
		int ReadPixel(uint32_t attachmentIndex, uint32_t x, uint32_t y) override;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { CHROMA_CORE_ASSERT("Index must be less than color attachment size!", index < m_ColorAttachments.size());  return m_ColorAttachments[index]; }
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
		void ClearAttachment(uint32_t attachmentIndex, int value) override;
	private:
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
		FramebufferTextureSpecification m_DepthAttachmentSpec;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}