#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <map>
#include <Chroma/Assets/Asset.h>
#include <Chroma/Images/Color.h>
#include <Chroma/Renderer/Texture.h>

#define ATLAS_VERSION 1

namespace Chroma
{
	class Font : public Asset
	{
		CHROMA_ASSET(Font, Asset);

	public:

		virtual ~Font() = default;
		
		struct Glyph
		{
			uint32_t codepoint;
			double l, b, r, t;
			double pl, pb, pr, pt;
		};

		struct Atlas
		{
			std::string font_name;

			int width;
			int height;
			int channels;

			double geometry_scale;
			double em_size;
			double ascender_y, decender_y;
			double line_height;
			double underline_y, underline_thickness;

			std::map<std::pair<uint32_t, uint32_t>, double> kerning;

			std::map<uint32_t, Glyph> glyphs;

			Ref<Texture2D> texture;
		};

		Atlas m_Atlas;
		std::unordered_map<char, Glyph *> m_Glyphs;

	protected:
		bool Load() override; //If no atlas, load font, create atlas
		bool Reload() override; //Load font, create or overwrite atlas
		bool Unload() override; //Unload font

		bool WriteAtlas(const std::string &path, std::vector<unsigned char> &atlas_data);
		bool ReadAtlas(Atlas *out, const std::string &path);

	private:
		

	};
}

