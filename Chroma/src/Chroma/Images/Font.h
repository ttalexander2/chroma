#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <Chroma/Images/Color.h>
#include <Chroma/Images/Image.h>

namespace Chroma
{
	class Font
	{
	public:
		
		struct Character
		{
			int glyph = 0;
			int width = 0;
			int height = 0;
			float advance = 0;
			float offset_x = 0;
			float offset_y = 0;
			float scale = 0;
			bool has_glyph = 0;
		};

		Font();
		Font(std::ifstream& stream);
		Font(const std::string& path);
		Font(const std::filesystem::path& path);
		Font(const Font&) = delete;
		Font& operator=(const Font&) = delete;
		Font(Font&& src) noexcept;
		Font& operator=(Font&& src) noexcept;
		~Font();

		void Dispose();

		const std::string& FamilyName() const;
		const std::string& StyleName() const;

		int Ascent() const;
		int Descent() const;
		int LineGap() const;
		int Height() const;
		int LineHeight() const;
		int GetGlyph(uint32_t codepoint) const;
		int GetScale(float size) const;
		float GetKerning(int glyph1, int glyph2, float scale) const;
		Character GetCharacter(int glyph, float scale) const;
		bool GetImage(const Character& ch, Color* pixels) const;
		Image GetImage(const Character& ch) const;
		bool IsValid() const;

	private:
		void Load(std::ifstream& stream);
		void* m_Font;
		unsigned char* m_Data;
		std::string m_FamilyName;
		std::string m_StyleName;
		int m_Ascent;
		int m_Descent;
		int m_LineGap;
		bool m_Valid;
	};
}

