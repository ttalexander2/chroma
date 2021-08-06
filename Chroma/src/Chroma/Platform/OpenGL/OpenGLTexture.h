#pragma once

#include "Chroma/Renderer/Texture.h"
#include <glad/glad.h>

namespace Chroma
{
	/// @brief OpenGL 2D Texture. Texutre can either read from one of the supported formats below,
	/// or can be constructed and assigned from a datastream.
	///
	/// Supported Formats:
	/// - JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
	///	- PNG 1 / 2 / 4 / 8 / 16 - bit - per - channel
	/// - TGA(not sure what subset, if a subset)
	/// - BMP non - 1bpp, non - RLE
	/// - PSD(composited view only, no extra channels, 8 / 16 bit - per - channel)
	/// - GIF(*comp always reports as 4 - channel)
	/// - HDR(radiance rgbE format)
	/// - PIC(Softimage PIC)
	/// - PNM(PPM and PGM binary only)
	/// - Animated GIF still needs a proper API, but here's one way to do it:
	/// 		http://gist.github.com/urraka/685d9a6340b26b830d49
	class OpenGLTexture2D : public Texture2D
	{
	public:
		/// @brief Constructs an OpenGLTexture2D.
		/// @param path Path to the texture.
		/// 
		/// This will load the texture from the disk and load it into GPU memory.
		/// Enable asserts to ensure sucessfull importing of textures.
		/// See above for supported image formats.
		OpenGLTexture2D(const std::string& path);

		/// @brief Constructs an empty OpenGLTexture2D.
		/// @param width Width of the texture.
		/// @param height Height of the texture.
		/// 
		/// This function will allocate space for the texure on the GPU.
		OpenGLTexture2D(uint32_t width, uint32_t height);

		/// @brief Destorys the texutre and frees the data on the GPU.
		virtual ~OpenGLTexture2D();

		/// @brief Gets the width of the texture.
		/// @return Width in pixels.
		virtual uint32_t GetWidth() const override { return m_Width; }

		/// @brief Gets the height of the texture.
		/// @return Height in pixels.
		virtual uint32_t GetHeight() const override { return m_Height; }

		/// @brief Binds the texture to a texture slot.
		/// @param slot Optional texture slot.
		virtual void Bind(uint32_t slot = 0) const override;

		/// @brief Sets the texture data.
		/// @param data Data to write to the GPU.
		/// @param size Size of the data to set.
		void SetData(void* data, uint32_t size) override;

		/// @brief Equality operator. Checks if two textures are identical.
		/// @param other Other texture to compare.
		/// @return Returns true if they are the same texture.
		virtual bool operator==(const Texture& other) const override 
		{ 
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID; 
		}

	private:
		/// @brief Path to the texture file
		std::string m_Path;
		/// @brief Size of the texture
		uint32_t m_Width, m_Height;
		/// @brief Renderer ID
		uint32_t m_RendererID;
		/// @brief Format of the texture
		GLenum m_InternalFormat, m_DataFormat;
	};
}


