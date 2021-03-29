#pragma once

#include "Chroma/Renderer/Framebuffer.h"

namespace Chroma
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferInfo& spec);
		virtual ~OpenGLFramebuffer() override;

		void Bind();
		void Unbind();
		void Resize();
		void Resize(uint32_t width, uint32_t height);

		virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }

		virtual const FramebufferInfo& GetSpecification() const override { return m_Specification; }

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
		FramebufferInfo m_Specification;
	};
}


