#pragma once

#include "Chroma/Core/Core.h"

namespace Chroma
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RGBA16F,
		RED_INTEGER,

		// Depth/Stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;

		FramebufferTextureSpecification(FramebufferTextureFormat format) :
			TextureFormat(format)
		{
		}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: Filtering/Wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;

		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments) :
			Attachments(attachments)
		{
		}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;

		FramebufferAttachmentSpecification Attachments;

		// Rendering to the swap chain
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		static Ref<Framebuffer> Create(const FramebufferSpecification &spec);

		virtual void Invalidate() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual int ReadPixel(uint32_t attachmentIndex, uint32_t x, uint32_t y) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual const FramebufferSpecification &GetSpecification() const = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
	};
}
