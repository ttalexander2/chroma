#pragma once
#include <Chroma/Images/Color.h>
#include <glm/ext/vector_int4.hpp>

namespace Chroma
{
	/// @brief RGBA image object.
	///
	/// Note: This is NOT a texture, pixel data is stored in memory.
	/// Use Texture2D for a texture stored in GPU memory.
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
	class Image
	{
	public:
		/// @brief Width of the texture in pixels.
		int Width = 0;

		/// @brief Height of the texture in pixels.
		int Height = 0;

		/// @brief Pointer to the pixel data.
		Color *Pixels = nullptr;

		/// @brief Construct an empty, unallocated Image.
		Image();

		/// @brief Construct a new Image object from a data stream.
		/// @param stream Datastream to read from.
		Image(std::fstream &stream);

		/// @brief File containing image file.
		/// @param file Path to the file.
		Image(const std::string &file);

		/// @brief Construct an empty image and allocate memory for data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		Image(int width, int height);

		/// @brief Construct an image from an existing image.
		/// @param other Image to copy.
		Image(const Image &other);

		/// @brief Image asignment operator.
		/// @param other Image to asign from.
		/// @return Image with matching data from other.
		Image &operator=(const Image &other);

		/// @brief Constructs an image from an existing image.
		/// @param other Image to asign from.
		Image(Image &&other) noexcept;

		/// @brief Image asignment operator.
		/// @param other Image to asign from.
		/// @return Image with matching data from other.
		Image &operator=(Image &&src) noexcept;

		/// Destorys and frees image data from memory.
		~Image();

		/// @brief Reads an image from a data stream.
		/// @param stream Stream to read from.
		/// @return Returns true if reading the imaga data was a success.
		bool FromStream(std::fstream &stream);

		/// @brief Disposes the image data, and frees the memory.
		void Dispose();

		/// @brief Premultiply the RGB channels of the image by the alpha value.
		void Premultiply();

		/// @brief Set the image's data from existing pixel data.
		/// @param rect Size of the image being provided.
		/// @param data Pixel data to copy.
		void SetData(const glm::ivec4 &rect, Color *data);

		/// @brief Save the image to a PNG file.
		/// @param file Path of the file to save.
		/// @return Returns true if successful.
		bool SavePNG(const std::string &file) const;

		/// @brief Save the image as a PNG to a data stream.
		/// @param stream Stream to write to.
		/// @return Returns true if successful.
		bool SavePNG(std::fstream &stream) const;

		/// @brief Save the image to a JPG file.
		/// @param file Path of the file to save.
		/// @return Returns true if successful.
		bool SaveJPG(const std::string &file, int quality) const;

		/// @brief Save the image as a JPG to a data stream.
		/// @param stream Stream to write to.
		/// @return Returns true if successful.
		bool SaveJPG(std::fstream &file, int quality) const;

		/// @brief Get a copy of the image's data. This function assumes memory is already allocated.
		/// @param dest Image data location to copy the image data to.
		/// @param dest_pos Destination position to start copying.
		/// @param dest_size Size of the destination image data.
		/// @param source_rect Subrectangle of the source wished to be copy.
		void GetData(Color *dest, const Math::vec2 &dest_pos, const Math::vec2 &dest_size, glm::ivec4 &source_rect);

		/// @brief Get a copy of the image's data. This function assumes memory is already allocated.
		/// @param dest 
		void GetData(Color *dest);

		/// @brief Flips the image vertically. This is useful since graphics APIs often render textures upside down.
		void FlipVertically();

		/// @brief Gets the total size of the data. (w*h)
		/// @return size of the data.
		size_t GetDataSize();

		/// @brief Calculates and returns a sub-image.
		/// @param source_rect Selection of image to return.
		/// @return Resulting sub-image.
		Image GetSubImage(const glm::ivec4 &source_rect);

	private:
		/// @brief Whether the image is owned (and therefor responsible for freeing) by stbi.
		bool m_Stbi_Ownership = false;
	};
}
