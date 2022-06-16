#include "chromapch.h"
#include "Font.h"

#include "Chroma/IO/FileSystem.h"
#include "Chroma/IO/Path.h"

#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdf-atlas-gen/image-encode.h>
#include <msdf-atlas-gen/json-export.h>
#include <stb_image.h>

#define CHROMA_FONT_MAGIC_NUMBER 124908323

namespace Chroma
{

	static Path GetCacheDirectory()
	{
		return Path("cache/fonts");
	}

	static const char *GetCacheExtension()
	{
		return ".font";
	}

	static bool CreateCacheDirectoryIfNeeded()
	{
		if (!Chroma::FileSystem::Exists(GetCacheDirectory()))
		{
			return Chroma::FileSystem::CreateDir(GetCacheDirectory());
		}
		return true;
	}

	bool Font::Load()
	{
		if (Chroma::FileSystem::Exists(GetCacheDirectory()))
		{
			Path cacheDir = GetCacheDirectory();
			cacheDir /= this->GetID().ToString() + GetCacheExtension();

			if (Chroma::FileSystem::Exists(cacheDir))
			{
				//Load shit from cache directory
				ReadAtlas(&m_Atlas, cacheDir);

				return true;
			}
		}
		
		return Reload();

	}

	bool Font::Reload()
	{
		using namespace msdf_atlas;

		bool success = false;
		if (msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype())
		{
			Chroma::File font_file = Chroma::File::Open(GetPath());
			if (!font_file.Good())
			{
				CHROMA_CORE_ERROR("Font could not be found!");
				msdfgen::deinitializeFreetype(ft);
				return false;
			}

			auto len = font_file.Length();
			std::vector<msdfgen::byte> data;
			data.resize(len);
			font_file.Read<msdfgen::byte>(data);
			font_file.Close();

			if (msdfgen::FontHandle *font = msdfgen::loadFontData(ft, data.data(), data.size()))
			{
				// Storage for glyph geometry and their coordinates in the atlas
				std::vector<GlyphGeometry> glyphs;
				// FontGeometry is a helper class that loads a set of glyphs from a single font.
				// It can also be used to get additional font metrics, kerning information, etc.
				FontGeometry fontGeometry(&glyphs);
				// Load a set of character glyphs:
				// The second argument can be ignored unless you mix different font sizes in one atlas.
				// In the last argument, you can specify a charset other than ASCII.
				// To load specific glyph indices, use loadGlyphs instead.
				fontGeometry.loadCharset(font, 1.0, Charset::ASCII, true, true);
				fontGeometry.loadMetrics(font, 1.0);
				fontGeometry.loadKerning(font);
				// Apply MSDF edge coloring. See edge-coloring.h for other coloring strategies.
				const double maxCornerAngle = 3.0;
				for (GlyphGeometry &glyph : glyphs)
					glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
				// TightAtlasPacker class computes the layout of the atlas.
				TightAtlasPacker packer;
				// Set atlas parameters:
				// setDimensions or setDimensionsConstraint to find the best value
				packer.setDimensionsConstraint(TightAtlasPacker::DimensionsConstraint::SQUARE);
				// setScale for a fixed size or setMinimumScale to use the largest that fits
				packer.setMinimumScale(48.0);
				// setPixelRange or setUnitRange
				packer.setPixelRange(2.0);
				packer.setMiterLimit(1.0);
				// Compute atlas layout - pack glyphs
				packer.pack(glyphs.data(), glyphs.size());
				// Get final atlas dimensions
				int width = 0, height = 0;
				packer.getDimensions(width, height);
				// The ImmediateAtlasGenerator class facilitates the generation of the atlas bitmap.

				ImmediateAtlasGenerator<float, 4, msdf_atlas::mtsdfGenerator, BitmapAtlasStorage<byte, 4>> generator(width, height);
				// GeneratorAttributes can be modified to change the generator's default settings.
				GeneratorAttributes attributes;
				generator.setAttributes(attributes);
				generator.setThreadCount(4);
				// Generate atlas bitmap
				generator.generate(glyphs.data(), glyphs.size());
				// The atlas bitmap can now be retrieved via atlasStorage as a BitmapConstRef.
				// The glyphs array (or fontGeometry) contains positioning data for typesetting text.

				m_Atlas.font_name = GetPath();
				m_Atlas.geometry_scale = fontGeometry.getGeometryScale();
				m_Atlas.em_size = fontGeometry.getMetrics().emSize;
				m_Atlas.ascender_y = fontGeometry.getMetrics().ascenderY;
				m_Atlas.decender_y = fontGeometry.getMetrics().descenderY;
				m_Atlas.line_height = fontGeometry.getMetrics().lineHeight;
				m_Atlas.underline_y = fontGeometry.getMetrics().underlineY;
				m_Atlas.underline_thickness = fontGeometry.getMetrics().underlineThickness;

				for (auto &first : fontGeometry.getGlyphs())
				{
					auto cp_first = first.getCodepoint();
					for (auto &second : fontGeometry.getGlyphs())
					{
						auto cp_second = second.getCodepoint();
						//Get codepoints
						double kerning = 0;
						fontGeometry.getAdvance(kerning, cp_first, cp_second);

						m_Atlas.kerning[std::pair(cp_first, cp_second)] = kerning;
					}
				}


				

				for (auto &fg_glyph : fontGeometry.getGlyphs())
				{
					Glyph glyph;
					glyph.codepoint = fg_glyph.getCodepoint();

					auto rect = fg_glyph.getBoxRect();
					fg_glyph.getQuadAtlasBounds(glyph.l, glyph.b, glyph.r, glyph.t);
					fg_glyph.getQuadPlaneBounds(glyph.pl, glyph.pb, glyph.pr, glyph.pt);

					m_Atlas.glyphs[glyph.codepoint] = glyph;
				}

				m_Atlas.texture = Texture2D::Create(width, height);
				byte *bitmap = (msdfgen::Bitmap<byte, 4>)generator.atlasStorage();
				m_Atlas.texture->SetData(bitmap, width * height * 4);

				std::vector<byte> png_data;
				if (encodePng(png_data, generator.atlasStorage()))
				{
					if (CreateCacheDirectoryIfNeeded())
					{
						Path cacheDir = GetCacheDirectory();
						cacheDir /= this->GetID().ToString() + GetCacheExtension();

						bool write_success = WriteAtlas(cacheDir, png_data);
						if (!write_success)
						{
							CHROMA_CORE_WARN("Failure occured when writing font atlas cache file!");
						}
					}
					else
					{
						CHROMA_CORE_WARN("Could not cache font. Cache directory could not be created or doesn't have permissions!");
						success = false;
					}
				}
				else
				{
					CHROMA_CORE_ERROR("Failed to encode font atlas.");
				}

				// Cleanup
				msdfgen::destroyFont(font);
			}
			else
			{
				CHROMA_CORE_ERROR("Failed to open font.");
			}
			msdfgen::deinitializeFreetype(ft);
		}
		else
		{
			CHROMA_CORE_ERROR("Failed to initialize freetype.");
		}

		return true;
	}

	bool Font::Unload()
	{
		m_Atlas.texture.reset();
		return true;
	}

	bool Font::WriteAtlas(const std::string &path, std::vector<unsigned char>& png_data)
	{
		File f = File::Open(path, Chroma::FileMode::Write);
		if (!f.Good())
			return false;

		bool success = true;

		//Font info
		f.Write(CHROMA_FONT_MAGIC_NUMBER);
		f.Write(ATLAS_VERSION);
		f.Write(m_Atlas.font_name.size());
		f.Write(m_Atlas.font_name);
		f.Write(m_Atlas.geometry_scale);
		f.Write(m_Atlas.em_size);
		f.Write(m_Atlas.ascender_y);
		f.Write(m_Atlas.decender_y);
		f.Write(m_Atlas.line_height);
		f.Write(m_Atlas.underline_y);
		f.Write(m_Atlas.underline_thickness);

		//Kerning
		f.Write(m_Atlas.kerning.size());
		for (auto &[pair, kerning] : m_Atlas.kerning)
		{
			f.Write(pair.first);
			f.Write(pair.second);
			f.Write(kerning);
		}

		//Glyphs
		f.Write(m_Atlas.glyphs.size());
		for (auto& [cp, glyph] : m_Atlas.glyphs)
		{
			f.Write(glyph.codepoint);
			f.Write(glyph.l);
			f.Write(glyph.b);
			f.Write(glyph.r);
			f.Write(glyph.t);
			f.Write(glyph.pl);
			f.Write(glyph.pb);
			f.Write(glyph.pr);
			f.Write(glyph.pt);
		}

		//Atlas
		f.Write(png_data.size());
		f.Write(png_data);

		f.Close();

		return true;
	}



	bool Font::ReadAtlas(Atlas *out, const std::string &path)
	{
		File f = File::Open(path, Chroma::FileMode::Read);
		if (!f.Good())
			return false;

		int magic_number = f.Read<int>();
		if (magic_number != CHROMA_FONT_MAGIC_NUMBER)
		{
			f.Close();
			return false;
		}

		int atlas_version = f.Read<int>();
		if (atlas_version == 1)
		{
			auto name_len = f.Read<size_t>();
			out->font_name.resize(name_len);
			f.Read(out->font_name.data(), name_len);

			out->geometry_scale = f.Read<double>();
			out->em_size = f.Read<double>();
			out->ascender_y = f.Read<double>();
			out->decender_y = f.Read<double>();
			out->line_height = f.Read<double>();
			out->underline_y = f.Read<double>();
			out->underline_thickness = f.Read<double>();

			size_t kerning_count = f.Read<size_t>();
			for (size_t i = 0; i < kerning_count; i++)
			{
				uint32_t cp_1 = f.Read<uint32_t>();
				uint32_t cp_2 = f.Read<uint32_t>();
				double kerning = f.Read<double>();

				out->kerning[std::pair(cp_1, cp_2)] = kerning;
			}

			size_t glyph_count = f.Read<size_t>();
			for (size_t i = 0; i < glyph_count; i++)
			{
				Glyph glyph{};

				glyph.codepoint = f.Read<uint32_t>();
				glyph.l = f.Read<double>();
				glyph.b = f.Read<double>();
				glyph.r = f.Read<double>();
				glyph.t = f.Read<double>();
				glyph.pl = f.Read<double>();
				glyph.pb = f.Read<double>();
				glyph.pr = f.Read<double>();
				glyph.pt = f.Read<double>();

				out->glyphs[glyph.codepoint] = glyph;
			}

			size_t atlas_size = f.Read<size_t>();

			std::vector<unsigned char> buffer;
			buffer.resize(atlas_size);

			f.Read(buffer.data(), atlas_size);

			stbi_set_flip_vertically_on_load(1);

			unsigned char *rawData = stbi_load_from_memory(buffer.data(), buffer.size(), &out->width, &out->height, &out->channels, 0);
			out->texture = Texture2D::Create(out->width, out->height);
			out->texture->SetData(rawData, out->width * out->height * 4);

		}


		f.Close();
		return true;
	}
}




#undef MAGIC_NUMBER