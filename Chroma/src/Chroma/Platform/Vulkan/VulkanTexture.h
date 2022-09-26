#pragma once

#include "Chroma/Renderer/Texture.h"

namespace Chroma
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(const std::string &path, bool flip_vertically);
		VulkanTexture2D(uint32_t width, uint32_t height);
		~VulkanTexture2D() override;

		uint32_t GetWidth() const override { return m_Width; }

		/// @brief Gets the height of the texture.
		/// @return Height in pixels.
		uint32_t GetHeight() const override { return m_Height; }

		void Bind(uint32_t slot = 0) const override;
		void SetData(void *data, uint32_t size) override;

		uint32_t GetTextureID() const override { return m_TextureID; }

		void SetTextureFormat(TextureFormat format) override
		{
		};

		void SetFiltering(FilterMethod method, FilterType type) override
		{
		};


		bool operator==(const Texture &other) const override
		{
			return false;
		}

	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_TextureID;
	};
}
