#pragma once

#include "Chroma/Core/Core.h"

namespace Chroma
{
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer()
		{
		};

		virtual void Bind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t *indices, uint32_t count);
	};
}
