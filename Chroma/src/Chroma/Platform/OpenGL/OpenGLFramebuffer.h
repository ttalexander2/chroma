#pragma once

#include "Chroma/Renderer/Framebuffer.h"

namespace Chroma
{
	/// @brief OpenGL Frame Buffer.
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		/// @brief Constructs a FrameBuffer given FrameBufferInfo.
		/// @param spec Specification of the frame buffer.
		OpenGLFramebuffer(const FramebufferInfo& spec);
		/// @brief Destorys the frame buffer.
		virtual ~OpenGLFramebuffer() override;

		/// @brief Binds the frame buffer.
		void Bind();
		/// @brief Unbinds the frame buffer.
		void Unbind();
		/// @brief Resizes the frame buffer given the width/height from the FramebufferInfo specification.
		void Resize();

		/// @brief Resizes the frame buffer to the specified size.
		/// @param width Width to resize.
		/// @param height Height to resize.
		void Resize(uint32_t width, uint32_t height);

		/// @brief Gets the Renderer ID for the color attachment.
		/// @return rendererID
		virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }

		/// @brief Gets the Frame Buffer Specification.
		/// @return Frame buffer's FramebufferInfo.
		virtual const FramebufferInfo& GetSpecification() const override { return m_Specification; }

	private:
		/// @brief Frame Buffer renderer id.
		uint32_t m_RendererID = 0;
		/// @brief Attachment renderer ids.
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
		/// @brief Frame buffer's specification.
		FramebufferInfo m_Specification;
	};
}


