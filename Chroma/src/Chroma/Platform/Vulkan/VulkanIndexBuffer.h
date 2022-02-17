#pragma once


#include "Chroma/Renderer/IndexBuffer.h"

namespace Chroma
{
	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t* indices, uint32_t count);
		~VulkanIndexBuffer();
		virtual void Bind() const override;
		virtual uint32_t GetCount() const override { return m_Count; }
	private:
		uint32_t m_Count;
	};
}