#include "chromapch.h"
#include "Font.h"

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION 
#include "stb_truetype.h"
#include <codecvt>
#include <locale>

namespace Chroma
{

	std::string GetFontName(stbtt_fontinfo* font, int nameID)
	{
		int len = 0;
		const wchar_t* ptr = (const wchar_t*)stbtt_GetFontNameString(font, &len,
			STBTT_PLATFORM_ID_MICROSOFT,
			STBTT_MS_EID_UNICODE_BMP,
			STBTT_MS_LANG_ENGLISH,
			nameID);

		// wide bois
		len /= 2;

		std::wstring str;
		if (len > 0)
			str.append(ptr, (size_t)len);

		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		return converter.to_bytes(str);
	}

	Font::Font()
	{
		m_Font = nullptr;
		m_Data = nullptr;
		m_Ascent = 0;
		m_Descent = 0;
		m_LineGap = 0;
		m_Valid = false;
	}

	Font::Font(std::ifstream& stream)
	{
		Load(stream);
	}

	Font::Font(const std::string& path)
	{
		if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
		{
			m_Font = nullptr;
			m_Data = nullptr;
			m_Ascent = 0;
			m_Descent = 0;
			m_LineGap = 0;
			m_Valid = false;
			return;
		}
		std::ifstream stream(path);
		if (stream.good())
			Load(stream);
	}

	Font::Font(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
		{
			m_Font = nullptr;
			m_Data = nullptr;
			m_Ascent = 0;
			m_Descent = 0;
			m_LineGap = 0;
			m_Valid = false;
			return;
		}
		std::ifstream stream(path);
		if (stream.good())
			Load(stream);
	}

	Font::Font(Font&& src) noexcept
	{
		m_Font = src.m_Font;
		m_Data = src.m_Data;
		m_FamilyName = src.m_FamilyName;
		m_StyleName = src.m_StyleName;
		m_Ascent = src.m_Ascent;
		m_Descent = src.m_Descent;
		m_LineGap = src.m_LineGap;
		m_Valid = src.m_Valid;

		src.m_FamilyName.clear();
		src.m_StyleName.clear();
		src.m_Valid = false;
		src.m_Font = nullptr;
		src.m_Data = nullptr;
	}

	Font& Font::operator=(Font&& src) noexcept
	{
		m_Font = src.m_Font;
		m_Data = src.m_Data;
		m_FamilyName = src.m_FamilyName;
		m_StyleName = src.m_StyleName;
		m_Ascent = src.m_Ascent;
		m_Descent = src.m_Descent;
		m_LineGap = src.m_LineGap;
		m_Valid = src.m_Valid;

		src.m_FamilyName.clear();
		src.m_StyleName.clear();
		src.m_Valid = false;
		src.m_Font = nullptr;
		src.m_Data = nullptr;
		
		return *this;
	}

	Font::~Font()
	{
		Dispose();
	}

	void Font::Dispose()
	{
		delete (stbtt_fontinfo*)m_Font;
		delete[] m_Data;
		m_Font = nullptr;
		m_Data = nullptr;
		m_FamilyName.clear();
		m_StyleName.clear();
	}

	const std::string& Font::FamilyName() const
	{
		return m_FamilyName;
	}

	const std::string& Font::StyleName() const
	{
		return m_StyleName;
	}

	int Font::Ascent() const
	{
		return m_Ascent;
	}

	int Font::Descent() const
	{
		return m_Descent;
	}

	int Font::LineGap() const
	{
		return m_LineGap;
	}

	int Font::Height() const
	{
		return m_Ascent - m_Descent;
	}

	int Font::LineHeight() const
	{
		return m_Ascent - m_Descent + m_LineGap;
	}

	int Font::GetGlyph(uint32_t codepoint) const
	{
		if (!m_Font)
			return 0;

		return stbtt_FindGlyphIndex((stbtt_fontinfo*)m_Font, codepoint);
	}

	int Font::GetScale(float size) const
	{
		if (!m_Font)
			return 0;
		return stbtt_ScaleForMappingEmToPixels((stbtt_fontinfo*)m_Font, size);
	}

	float Font::GetKerning(int glyph1, int glyph2, float scale) const
	{
		if (!m_Font)
			return 0;
		return stbtt_GetGlyphKernAdvance((stbtt_fontinfo*)m_Font, glyph1, glyph2) * scale;
	}

	Font::Character Font::GetCharacter(int glyph, float scale) const
	{
		Character ch;

		if (!m_Font)
			return ch;

		int advance, offsetX, x0, y0, x1, y1;

		stbtt_GetGlyphHMetrics((stbtt_fontinfo*)m_Font, glyph, &advance, &offsetX);
		stbtt_GetGlyphBitmapBox((stbtt_fontinfo*)m_Font, glyph, scale, scale, &x0, &y0, &x1, &y1);

		int w = x1 - x0;
		int h = y1 - y0;

		ch.glyph = glyph;
		ch.width = w;
		ch.height = h;
		ch.advance = advance * scale;
		ch.offset_x = offsetX * scale;
		ch.offset_y = (float)y0;
		ch.scale = scale;
		ch.has_glyph = (w > 0 && h > 0 && stbtt_IsGlyphEmpty((stbtt_fontinfo*)m_Font, glyph) == 0);

		return ch;

	}

	bool Font::GetImage(const Character& ch, Color* pixels) const
	{
		if (ch.has_glyph)
		{
			auto* src = (unsigned char*)pixels;
			stbtt_MakeGlyphBitmap((stbtt_fontinfo*)m_Font, src, ch.width, ch.height, ch.width, ch.scale, ch.scale, ch.glyph);

			int len = ch.width * ch.height;
			for (int a = (len - 1) * 4, b = (len - 1); b >= 0; a -= 4, b -= 1)
			{
				src[a + 0] = src[b];
				src[a + 1] = src[b];
				src[a + 2] = src[b];
				src[a + 3] = src[b];
			}
			return true;
		}
		return false;
	}

	Image Font::GetImage(const Character& ch) const
	{
		Image img(ch.width, ch.height);
		if (GetImage(ch, img.Pixels))
			return img;
		return Image();
	}

	bool Font::IsValid() const
	{
		return m_Valid;
	}

	void Font::Load(std::ifstream& stream)
	{
		Dispose();

		if (!stream.good())
		{
			CHROMA_CORE_ERROR("Unable to load font. Stream unreadable!");
			return;
		}

		stream.seekg(0, std::ios::beg);
		auto beg = stream.tellg();
		stream.seekg(0, std::ios::end);
		auto size = stream.tellg() - beg;
		stream.seekg(0, std::ios::beg);
		m_Data = new unsigned char[size];
		stream.read((char*)m_Data, size);

		m_Font = new stbtt_fontinfo();
		auto fn = (stbtt_fontinfo*)m_Font;
		stbtt_InitFont(fn, m_Data, 0);
		m_FamilyName = GetFontName(fn, 1);
		m_StyleName = GetFontName(fn, 2);

		stbtt_GetFontVMetrics(fn, &m_Ascent, &m_Descent, &m_LineGap);
		m_Valid = true;

	}
}


