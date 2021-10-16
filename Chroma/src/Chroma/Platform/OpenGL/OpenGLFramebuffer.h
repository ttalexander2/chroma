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
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		/// @brief Destorys the frame buffer.
		virtual ~OpenGLFramebuffer() override;

		/// @brief Binds the frame buffer.
		void Bind() override;
		/// @brief Unbinds the frame buffer.
		void Unbind() override;
		/// @brief Resizes the frame buffer given the width/height from the FramebufferInfo specification.
		void Invalidate() override;

		/// @brief Resizes the frame buffer to the specified size.
		/// @param width Width to resize.
		/// @param height Height to resize.
		void Resize(uint32_t width, uint32_t height) override;

		int ReadPixel(uint32_t attachmentIndex, uint32_t x, uint32_t y) override;

		/// @brief Gets the Renderer ID for the color attachment.
		/// @return rendererID
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { CHROMA_CORE_ASSERT("Index must be less than color attachment size!", index < m_ColorAttachments.size());  return m_ColorAttachments[index]; }

		/// @brief Gets the Frame Buffer Specification.
		/// @return Frame buffer's FramebufferInfo.
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

		void ClearAttachment(uint32_t attachmentIndex, int value) override;

	private:
		/// @brief Frame Buffer renderer id.
		uint32_t m_RendererID = 0;
		/// @brief Frame buffer's specification.
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
		FramebufferTextureSpecification m_DepthAttachmentSpec;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}


