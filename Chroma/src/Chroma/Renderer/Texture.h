#pragma once

#include <Chroma/Core/Core.h>
#include <string>

namespace Chroma
{
	class Texture
	{
	public:

		enum class TextureFormat
		{
			RGB8,
			RGBA8
		};

		enum class FilterMethod
		{
			LINEAR,
			NEAREST
		};

		enum class FilterType
		{
			MIN,
			MAG
		};

		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual bool operator==(const Texture& other) const = 0;

		virtual uint32_t GetTextureID() const = 0;

		virtual void SetTextureFormat(TextureFormat format) = 0;
		virtual void SetFiltering(FilterMethod method, FilterType type) = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path, bool flip_vertically = true);

	};
}
