#pragma once
#include <Chroma/Images/Color.h>
#include <glm/ext/vector_int4.hpp>

namespace Chroma
{
	// This is NOT a texture, it stores pixel data in memory.
	// Use Texture2D for a texture stored in GPU memory.
	class Image
	{
	public:
		int Width = 0;
		int Height = 0;

		Color* Pixels = nullptr;

		Image();
		Image(std::fstream& stream);
		Image(const std::string& file);
		Image(int width, int height);
		Image(const Image& other);
		Image& operator=(const Image& other);
		Image(Image&& other) noexcept;
		Image& operator=(Image&& src) noexcept;
		~Image();

		bool FromStream(std::fstream& stream);
		void Dispose();
		void Premultiply();
		void SetData(const glm::ivec4& rect, Color* data);
		bool SavePNG(const std::string& file) const;
		bool SavePNG(std::fstream& stream) const;
		bool SaveJPG(const std::string& file, int quality) const;
		bool SaveJPG(std::fstream& file, int quality) const;
		void GetData(Color* dest, const Math::vec2& dest_pos, const Math::vec2& dest_size, glm::ivec4& source_rect);
		void GetData(Color* dest);
		void FlipVertically();
		size_t GetDataSize();
		Image GetSubImage(const glm::ivec4& source_rect);

	private:
		bool m_Stbi_Ownership;


	};
}


