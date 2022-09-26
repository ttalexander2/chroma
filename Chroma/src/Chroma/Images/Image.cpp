#include "chromapch.h"
#include "Image.h"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


namespace Chroma
{

	int stbi_read_callback(void* user, char* data, int size)
	{
		((std::fstream*)user)->read(data, std::streamsize(size));
		return (int)((std::fstream*)user)->tellg();
	}

	void stbi_skip_callback(void* user, int n)
	{
		((std::fstream*)user)->seekg(n, std::ios::cur);
	}

	int stbi_eof_callback(void* user)
	{
		int64_t position = ((std::fstream*)user)->tellg();
		((std::fstream*)user)->seekg(0, std::ios::end);
		int64_t length = ((std::fstream*)user)->tellg();
		((std::fstream*)user)->seekg(position, std::ios::beg);

		if (position >= length)
			return 1;
		return 0;
	}

	void stbi_write_callback(void* context, void* data, int size)
	{
		((std::fstream*)context)->write((char*)data, size);
	}

	Image::Image()
	{
		Width = 0;
		Height = 0;
		Pixels = nullptr;
		m_Stbi_Ownership = false;
	}

	Image::Image(std::fstream& stream)
	{
		Width = 0;
		Height = 0;
		Pixels = nullptr;
		m_Stbi_Ownership = false;
		FromStream(stream);
	}

	Image::Image(const std::string& file)
	{
		Width = 0;
		Height = 0;
		Pixels = nullptr;
		m_Stbi_Ownership = false;

		std::fstream fs (file, std::fstream::binary | std::fstream::in);
		if (fs.good())
			FromStream(fs);
	}

	Image::Image(int width, int height)
	{
		CHROMA_CORE_ASSERT(width >= 0 && height >= 0, "Image size must be greater than 0");

		Width = width;
		Height = height;
		Pixels = new Color[width * height];
		m_Stbi_Ownership = false;
		memset(Pixels, 0, (size_t)width * (size_t)height * sizeof(Color));
	}

	Image::Image(const Image& src)
	{
		Width = src.Width;
		Height = src.Height;
		m_Stbi_Ownership = src.m_Stbi_Ownership;
		Pixels = nullptr;

		if (src.Pixels != nullptr && Width >= 0 && Height >= 0)
		{
			Pixels = new Color[Width * Height];
			memcpy(Pixels, src.Pixels, sizeof(Color) * Width * Height);
		}
	}

	Image& Image::operator=(const Image& src)
	{
		Dispose();

		Width = src.Width;
		Height = src.Height;
		m_Stbi_Ownership = src.m_Stbi_Ownership;
		Pixels = nullptr;

		if (src.Pixels != nullptr && Width >= 0 && Height >= 0)
		{
			Pixels = new Color[Width * Height];
			memcpy(Pixels, src.Pixels, sizeof(Color) * Width * Height);
		}

		return *this;
	}

	Image::Image(Image&& src) noexcept
	{
		Width = src.Width;
		Height = src.Height;
		Pixels = src.Pixels;
		m_Stbi_Ownership = src.m_Stbi_Ownership;
		src.Width = src.Height = 0;
		src.Pixels = nullptr;
		src.m_Stbi_Ownership = false;
	}

	Image& Image::operator=(Image&& src) noexcept
	{
		Dispose();

		Width = src.Width;
		Height = src.Height;
		Pixels = src.Pixels;
		m_Stbi_Ownership = src.m_Stbi_Ownership;
		src.Width = src.Height = 0;
		src.Pixels = nullptr;
		src.m_Stbi_Ownership = false;

		return *this;
	}

	Image::~Image()
	{
		Dispose();
	}

	bool Image::FromStream(std::fstream& stream)
	{
		Dispose();

		if (!stream.good())
			return false;

		stbi_io_callbacks callbacks;
		callbacks.eof = stbi_eof_callback;
		callbacks.read = stbi_read_callback;
		callbacks.skip = stbi_skip_callback;

		int x, y, comps;
		uint8_t* data = stbi_load_from_callbacks(&callbacks, &stream, &x, &y, &comps, 4);

		if (data == nullptr)
			return false;

		m_Stbi_Ownership = true;
		Pixels = (Color*)data;
		Width = x;
		Height = y;

		return true;
	}

	void Image::Dispose()
	{
		if (m_Stbi_Ownership)
			stbi_image_free(Pixels);
		else
			delete[] Pixels;

		Pixels = nullptr;
		Width = Height = 0;
		m_Stbi_Ownership = false;
	}

	void Image::Premultiply()
	{
		if (Pixels != nullptr)
		{
			for (int i = 0; i < Width * Height; i++)
			{
				Pixels[i].r = (uint8_t)(Pixels[i].r * Pixels[i].a / 255);
				Pixels[i].g = (uint8_t)(Pixels[i].g * Pixels[i].a / 255);
				Pixels[i].b = (uint8_t)(Pixels[i].b * Pixels[i].a / 255);
			}
		}
	}

	void Image::SetData(const glm::ivec4& rect, Color* data)
	{
		for (int y = 0; y < rect.w; y++)
		{
			int to = rect.x + ((rect.y + y) * Width);
			int from(y * rect.z);
			memcpy(Pixels + to, data + from, sizeof(Color) * rect.z);
		}
	}

	bool Image::SavePNG(const std::string& file) const
	{
		std::fstream fs(file, std::fstream::binary | std::fstream::out);
		bool result = SavePNG(fs);
		if (!result) CHROMA_CORE_ERROR("Error loading stream: \'{}\'", file);
		else fs.close();
		return result;
	}

	bool Image::SavePNG(std::fstream& stream) const
	{
		CHROMA_CORE_ASSERT(Pixels != nullptr, "Image pixel date cannot be nullptr");
		CHROMA_CORE_ASSERT(Width > 0 && Height > 0, "Image size must be larger than 0");

		if (stream.good())
		{
			stbi_write_force_png_filter = 0;
			stbi_write_png_compression_level = 0;

			if (stbi_write_png_to_func(stbi_write_callback, &stream, Width, Height, 4, Pixels, Width * 4) != NULL)
				return true;
		}

		CHROMA_CORE_ERROR("Could not write image to png!");

		return false;
	}

	bool Image::SaveJPG(const std::string& file, int quality) const
	{
		std::fstream fs(file, std::fstream::binary | std::fstream::out);
		return SaveJPG(fs, quality);
	}

	bool Image::SaveJPG(std::fstream& stream, int quality) const
	{
		CHROMA_CORE_ASSERT(Pixels != nullptr, "Image pixel date cannot be nullptr");
		CHROMA_CORE_ASSERT(Width > 0 && Height > 0, "Image size must be larger than 0");

		if (quality < 1)
		{
			CHROMA_CORE_WARN("JPG quality should be between 1 and 100; quality was {0}", quality);
			quality = 1;
		}
		else if (quality > 100)
		{
			CHROMA_CORE_WARN("JPG quality should be between 1 and 100; quality was {0}", quality);
			quality = 100;
		}

		if (stream.good())
		{

			if (stbi_write_jpg_to_func(stbi_write_callback, &stream, Width, Height, 4, Pixels, quality) != NULL)
				return true;
		}

		return false;
	}

	size_t Image::GetDataSize()
	{
		return sizeof(Pixels);
	}

	void Image::GetData(Color* dest)
	{
		memcpy(dest, Pixels, sizeof(Color) * Width * Height);
	}

	void Image::FlipVertically()
	{
		Color* temp = new Color[Width * Height];
		memcpy(temp, Pixels, sizeof(Color) * Width * Height);

		for (int y = 0; y < Height; y++)
		{
			int to = (Width * Height - Width) - (y * Width);
			int from = (y * Width);
			memcpy(Pixels + to, temp + from, sizeof(Color) * Width);
		}

		delete[] temp;
	}

	void Image::GetData(Color* dest, const Math::vec2& dest_pos, const Math::vec2& dest_size, glm::ivec4& source_rect)
	{
		if (source_rect.x > 0) source_rect.x = 0;
		if (source_rect.y > 0) source_rect.y = 0;
		if (source_rect.x + source_rect.z > Width) source_rect.z = Width - source_rect.x;
		if (source_rect.y + source_rect.w > Height) source_rect.w = Width - source_rect.y;

		if (source_rect.z > dest_size.x - dest_pos.x)
			source_rect.z = static_cast<int>(dest_size.x - dest_pos.x);
		if (source_rect.w > dest_size.y - dest_pos.y)
			source_rect.w = static_cast<int>(dest_size.y - dest_pos.y);

		for (int y = 0; y < source_rect.w; y++)
		{
			int to = static_cast<int>(dest_pos.x + (dest_pos.y + y) * dest_size.x);
			int from = source_rect.x + (source_rect.y + y) * Width;
			memcpy(dest + to, Pixels + from, sizeof(Color) * (int)source_rect.z);

		}
	}

	Image Image::GetSubImage(const glm::ivec4& source_rect)
	{
		glm::ivec4 rect(source_rect);
		Image img(source_rect.z, source_rect.w);
		GetData(img.Pixels, { 0,0 }, { img.Width, img.Height }, rect);
		return img;
	}



}