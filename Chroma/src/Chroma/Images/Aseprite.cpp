#include "chromapch.h"
#include "Aseprite.h"

#define STBI_NO_STDIO
#define STBI_ONLY_ZLIB
#include "stb_image.h"

#include <fstream>
#include "BlendFunctions.h"



namespace Chroma
{


	Aseprite::Aseprite()
		= default;

	Aseprite::Aseprite(const std::string& path)
	{
		std::fstream fs(path, std::fstream::in | std::fstream::binary);
		parse(fs);
		fs.close();
	}

	Aseprite::Aseprite(std::fstream & stream)
	{
		parse(stream);
	}

	Aseprite::Aseprite(const Aseprite & src)
	{
		mode = src.mode;
		width = src.width;
		height = src.height;
		layers = src.layers;
		frames = src.frames;
		tags = src.tags;
		slices = src.slices;
		palette = src.palette;
	}

	Aseprite::Aseprite(Aseprite && src) noexcept
	{
		mode = src.mode;
		width = src.width;
		height = src.height;
		layers = std::move(src.layers);
		frames = std::move(src.frames);
		tags = std::move(src.tags);
		slices = std::move(src.slices);
		palette = std::move(src.palette);
	}

	Aseprite& Aseprite::operator=(const Aseprite & src)
	{
		mode = src.mode;
		width = src.width;
		height = src.height;
		layers = src.layers;
		frames = src.frames;
		tags = src.tags;
		slices = src.slices;
		palette = src.palette;
		return *this;
	}

	Aseprite& Aseprite::operator=(Aseprite && src) noexcept
	{
		mode = src.mode;
		width = src.width;
		height = src.height;
		layers = std::move(src.layers);
		frames = std::move(src.frames);
		tags = std::move(src.tags);
		slices = std::move(src.slices);
		palette = std::move(src.palette);
		return *this;
	}

	Aseprite::~Aseprite()
	{
		Dispose();
	}

	void Aseprite::Dispose()
	{
		for (Frame f : frames)
		{
			f.image.Dispose();
			for (Cel c : f.cels)
			{
				c.image.Dispose();
			}
		}
		frames.clear();
		layers.clear();
		tags.clear();
		slices.clear();
		palette.clear();
	}

	void Aseprite::parse(std::fstream & stream)
	{
		if (!stream.good())
		{
			CHROMA_CORE_ASSERT(false, "std::fstream is not readable");
			return;
		}

		int frame_count = 0;

		// header
		{
			// filesize
			read<uint32_t>(stream, Endian::Little);

			// magic number
			auto magic = read<uint16_t>(stream, Endian::Little);
			if (magic != 0xA5E0)
			{
				CHROMA_CORE_ASSERT(false, "File is not a valid Aseprite file");
				return;
			}

			// main info
			frame_count = read<uint16_t>(stream, Endian::Little);
			width = read<uint16_t>(stream, Endian::Little);
			height = read<uint16_t>(stream, Endian::Little);
			mode = static_cast<Aseprite::Modes>(read<uint16_t>(stream, Endian::Little) / 8);

			// don't care about other info
			read<uint32_t>(stream, Endian::Little);				// Flags
			read<uint16_t>(stream, Endian::Little);				// Speed (deprecated)
			read<uint32_t>(stream, Endian::Little);				// Should be 0
			read<uint32_t>(stream, Endian::Little);				// Should be 0
			read<uint8_t>(stream, Endian::Little);				// Palette entry
			stream.seekg(3, std::ios_base::cur);				// Ignore these bytes
			read<uint16_t>(stream, Endian::Little);				// Number of colors (0 means 256 for old sprites)
			read<int8_t>(stream, Endian::Little);				// Pixel width
			read<int8_t>(stream, Endian::Little);				// Pixel height
			stream.seekg(92, std::ios_base::cur);				// For Future
		}

		stream.seekg(128, std::ios_base::beg);

		frames.resize(frame_count);

		//CHROMA_CORE_TRACE("Frame Count: {0}", frame_count);

		// frames
		for (int i = 0; i < frame_count; i++)
		{
			auto frameStart = stream.tellg();

			auto offset = read<uint32_t>(stream, Endian::Little);

			stream.seekg((std::streamoff)offset - 4, std::ios_base::cur);
			auto frameEnd = stream.tellg();

			stream.seekg(frameStart);

			stream.seekg(4, std::ios_base::cur);

			unsigned int chunks = 0;

			// frame header
			{
				auto magic = read<uint16_t>(stream, Endian::Little); // magic number
				if (magic != 0xF1FA)
				{
					CHROMA_CORE_ASSERT(false, "File is not a valid Aseprite file");
					return;
				}

				auto old_chunk_count = read<uint16_t>(stream, Endian::Little);
				frames[i].duration = read<uint16_t>(stream, Endian::Little);
				stream.seekg(2, std::ios_base::cur); // for future
				auto new_chunk_count = read<uint32_t>(stream, Endian::Little);

				if (old_chunk_count == 0xFFFF)
					chunks = new_chunk_count;
				else
					chunks = old_chunk_count;
			}

			// make frame image
			frames[i].image = Image(width, height);

			//CHROMA_CORE_TRACE("Frame [{}]({} chunks).", i, chunks);

			// frame chunks
			for (unsigned int j = 0; j < chunks; j++)
			{
				auto chunkStart = stream.tellg();
				uint32_t chunkSize = read<uint32_t>(stream, Endian::Little);
				stream.seekg((std::streamoff)chunkSize - 4, std::ios_base::cur);
				auto chunkEnd = stream.tellg();
				stream.seekg(chunkStart);
				stream.seekg(4, std::ios_base::cur);

				auto chunkType = static_cast<Chunks>(read<uint16_t>(stream, Endian::Little));

				//CHROMA_CORE_TRACE("Chunk [{}]: Start[{}], End[{}], Size[{}], Type[0x{:x}]", j, (std::streamoff)chunkStart, (std::streamoff)chunkEnd, chunkSize, chunkType);



				switch (chunkType)
				{
					case Chunks::Layer: parse_layer(stream, i); break;
					case Chunks::Cel: parse_cel(stream, i, (std::streamoff)chunkEnd); break;
					case Chunks::Palette: parse_palette(stream, i); break;
					case Chunks::UserData: parse_user_data(stream, i); break;
					case Chunks::FrameTags: parse_tag(stream, i); break;
					case Chunks::Slice: parse_slice(stream, i); break;
					default: break;
				}

				stream.seekg(chunkEnd);
			}

			stream.seekg(frameEnd);
		}
	}

	void Aseprite::parse_layer(std::fstream & stream, int frame)
	{
		layers.emplace_back();

		auto& layer = layers.back();
		layer.flag = static_cast<LayerFlags>(read<uint16_t>(stream, Endian::Little));
		layer.visible = ((int)layer.flag & (int)LayerFlags::Visible) == (int)LayerFlags::Visible;
		layer.type = static_cast<LayerTypes>(read<uint16_t>(stream, Endian::Little));
		layer.child_level = read<uint16_t>(stream, Endian::Little);
		read<uint16_t>(stream, Endian::Little); // width
		read<uint16_t>(stream, Endian::Little); // height
		int blendMode = read<uint16_t>(stream, Endian::Little);
		if (blendMode < 0 || blendMode > 18)
			blendMode = 0;
		layer.blendmode = static_cast<BlendMode>(blendMode);
		layer.alpha = read<uint8_t>(stream, Endian::Little);
		stream.seekg(3, std::ios_base::cur); // for future

		char* buff = new char[read<uint16_t>(stream, Endian::Little)];
		stream.read(buff, layer.name.length());
		layer.name = std::string(buff);
		delete[] buff;

		layer.userdata.color = {255, 255, 255, 1};
		layer.userdata.text = "";
		m_last_userdata = &(layer.userdata);
	}

	void Aseprite::parse_cel(std::fstream & stream, int frameIndex, size_t maxPosition)
	{
		Frame& frame = frames[frameIndex];

		frame.cels.emplace_back();
		auto& cel = frame.cels.back();
		cel.layer_index = read<uint16_t>(stream, Endian::Little);
		cel.x = read<uint16_t>(stream, Endian::Little);
		cel.y = read<uint16_t>(stream, Endian::Little);
		cel.alpha = read<uint8_t>(stream, Endian::Little);
		cel.linked_frame_index = -1;

		auto celType = read<uint16_t>(stream, Endian::Little);
		stream.seekg(7, std::ios_base::cur);

		// RAW or DEFLATE
		if (celType == 0 || celType == 2)
		{
			auto width = read<uint16_t>(stream, Endian::Little);
			auto height = read<uint16_t>(stream, Endian::Little);
			auto count = width * height * (int)mode;

			cel.image = Image(width, height);

			// RAW
			if (celType == 0)
			{
				stream.read(reinterpret_cast<char*>(cel.image.Pixels), count);
				//stream.read(cel.image.Pixels, count);
			}
			// DEFLATE (zlib)
			else
			{
				// this could be optimized to use a buffer on the stack if we only read set chunks at a time
				// stbi's zlib doesn't have that functionality though
				auto size = maxPosition - (std::streamoff)stream.tellg();
				if (size > INT32_MAX)
					size = INT32_MAX;

				char* buffer = new char[size];
				stream.read(buffer, size);

				int olen = width * height * sizeof(Math::vec4);
				int res = stbi_zlib_decode_buffer(reinterpret_cast<char*>(cel.image.Pixels), olen, buffer, (int)size);

				delete[] buffer;

				if (res < 0)
				{
					CHROMA_CORE_ASSERT(false, "Unable to parse Aseprite file");
					return;
				}
			}

			// convert to pixels
			// note: we work in-place to save having to store stuff in a buffer
			if (mode == Modes::Grayscale)
			{
				auto src = (unsigned char*)cel.image.Pixels;
				auto dst = cel.image.Pixels;
				for (int d = width * height - 1, s = (width * height - 1) * 2; d >= 0; d--, s -= 2)
					dst[d] = Color(src[s], src[s], src[s], src[s + 1]);
			}
			else if (mode == Modes::Indexed)
			{
				auto src = (unsigned char*)cel.image.Pixels;
				auto dst = cel.image.Pixels;
				for (int i = width * height - 1; i >= 0; i--)
					dst[i] = palette[src[i]];
			}

		}
		// REFERENCE
		// this cel directly references a previous cel
		else if (celType == 1)
		{
			cel.linked_frame_index = read<uint16_t>(stream, Endian::Little);
		}

		// draw to frame if visible
		if ((int)layers[cel.layer_index].flag & (int)LayerFlags::Visible)
		{
			render_cel(&cel, &frame);
		}

		cel.userdata.color = 0xffffff;
		cel.userdata.text = "";
		m_last_userdata = &(cel.userdata);
	}

	void Aseprite::parse_palette(std::fstream & stream, int frame)
	{
		/* size */ read<uint32_t>(stream, Endian::Little);
		auto start = read<uint32_t>(stream, Endian::Little);
		auto end = read<uint32_t>(stream, Endian::Little);
		stream.seekg(8, std::ios_base::cur);

		palette.resize(palette.size() + (end - start + 1));

		for (int p = 0, len = static_cast<int>(end - start) + 1; p < len; p++)
		{
			auto hasName = read<uint16_t>(stream, Endian::Little);
			palette[start + p] = read<uint32_t>(stream, Endian::Little);

			if (hasName & 0xF000)
			{
				int len = read<uint16_t>(stream, Endian::Little);
				stream.seekg(len, std::ios_base::cur);
			}
		}
	}

	void Aseprite::parse_user_data(std::fstream & stream, int frame)
	{
		if (m_last_userdata != nullptr)
		{
			auto flags = read<uint32_t>(stream, Endian::Little);

			// has text
			if (flags & (1 << 0))
			{
				char* buff = new char[read<uint16_t>(stream, Endian::Little)];
				stream.read(buff, m_last_userdata->text.length());
				m_last_userdata->text = std::string(buff);
				delete[] buff;
			}

			// has color
			if (flags & (1 << 1))
				m_last_userdata->color = read<uint32_t>(stream, Endian::Little);
		}
	}

	void Aseprite::parse_tag(std::fstream & stream, int frame)
	{
		auto count = read<uint16_t>(stream, Endian::Little);
		stream.seekg(8, std::ios_base::cur);

		for (int t = 0; t < count; t++)
		{
			Tag tag;
			tag.from = read<uint16_t>(stream, Endian::Little);
			tag.to = read<uint16_t>(stream, Endian::Little);
			tag.loops = static_cast<LoopDirections>(read<int8_t>(stream, Endian::Little));

			stream.seekg(8, std::ios_base::cur);
			tag.color = Color(read<int8_t>(stream), read<int8_t>(stream), read<int8_t>(stream, Endian::Little), 255);
			stream.seekg(1, std::ios_base::cur);

			uint16_t len = read<uint16_t>(stream, Endian::Little);
			char* buff = new char[len];
			stream.read(buff, len);
			tag.name = std::string(buff, buff + len);
			delete[] buff;

			tags.push_back(tag);
		}
	}

	void Aseprite::parse_slice(std::fstream & stream, int frame)
	{
		int count = read<uint32_t>(stream, Endian::Little);
		int flags = read<uint32_t>(stream, Endian::Little);
		read<uint32_t>(stream, Endian::Little); // reserved

		std::string name;
		char* buff = new char[read<uint16_t>(stream, Endian::Little)];
		stream.read(buff, name.length());
		name = std::string(buff);
		delete[] buff;

		for (int s = 0; s < count; s++)
		{
			slices.emplace_back();

			auto& slice = slices.back();
			slice.name = name;
			slice.frame = read<uint32_t>(stream, Endian::Little);
			slice.origin.x = read<int32_t>(stream, Endian::Little);
			slice.origin.y = read<int32_t>(stream, Endian::Little);
			slice.width = read<uint32_t>(stream, Endian::Little);
			slice.height = read<uint32_t>(stream, Endian::Little);

			// 9 slice (ignored atm)
			if (flags & (1 << 0))
			{
				read<int32_t>(stream, Endian::Little);
				read<int32_t>(stream, Endian::Little);
				read<uint32_t>(stream, Endian::Little);
				read<uint32_t>(stream, Endian::Little);
			}

			// pivot point
			slice.has_pivot = false;
			if (flags & (1 << 1))
			{
				slice.has_pivot = true;
				slice.pivot.x = read<uint32_t>(stream, Endian::Little);
				slice.pivot.y = read<uint32_t>(stream, Endian::Little);
			}

			slice.userdata.color = 0xffffff;
			slice.userdata.text = "";
			m_last_userdata = &(slice.userdata);
		}
	}

#define MUL_UN8(a, b, t) \
	((t) = (a) * (uint16_t)(b) + 0x80, ((((t) >> 8) + (t) ) >> 8))

	void Aseprite::render_cel(Cel * cel, Frame * frame)
	{
		Layer& layer = layers[cel->layer_index];

		while (cel->linked_frame_index >= 0)
		{
			auto& frame = frames[cel->linked_frame_index];
			for (auto& it : frame.cels)
				if (it.layer_index == cel->layer_index)
				{
					cel = &it;
					break;
				}
		}

		int t;
		unsigned char opacity = MUL_UN8(cel->alpha, layer.alpha, t);
		if (opacity <= 0)
			return;

		auto src = cel->image.Pixels;
		auto srcX = cel->x;
		auto srcY = cel->y;
		auto srcW = cel->image.Width;
		auto srcH = cel->image.Height;
		auto dst = frame->image.Pixels;
		auto dstW = frame->image.Width;
		auto dstH = frame->image.Height;

		// blit pixels
		int left = Math::max(0, srcX);
		int right = Math::min(dstW, srcX + srcW);
		int top = Math::max(0, srcY);
		int bottom = Math::min(dstH, srcY + srcH);

		//Normal

		for (int dx = left, sx = -Math::min(srcX, 0); dx < right; dx++, sx++)
		{
			for (int dy = top, sy = -Math::min(srcY, 0); dy < bottom; dy++, sy++)
			{
				Chroma::Color* srcColor = (src + sx + sy * srcW);
				Chroma::Color* dstColor = (dst + dx + dy * dstW);

				if (srcColor->a != 0)
				{
					switch (layer.blendmode)
					{
						case BlendMode::Normal: BlendFunctions::BlendNormal(dstColor, srcColor, opacity); break;
						case BlendMode::Multiply: BlendFunctions::BlendMultiply(dstColor, srcColor, opacity); break;
						case BlendMode::Screen: BlendFunctions::BlendScreen(dstColor, srcColor, opacity); break;
						case BlendMode::Overlay: BlendFunctions::BlendOverlay(dstColor, srcColor, opacity); break;
						case BlendMode::Darken: BlendFunctions::BlendDarken(dstColor, srcColor, opacity); break;
						case BlendMode::Lighten: BlendFunctions::BlendLighten(dstColor, srcColor, opacity); break;
						case BlendMode::ColorDodge: BlendFunctions::BlendColorDodge(dstColor, srcColor, opacity); break;
						case BlendMode::ColorBurn: BlendFunctions::BlendColorBurn(dstColor, srcColor, opacity); break;
						case BlendMode::HardLight: BlendFunctions::BlendHardLight(dstColor, srcColor, opacity); break;
						case BlendMode::SoftLight: BlendFunctions::BlendSoftLight(dstColor, srcColor, opacity); break;
						case BlendMode::Difference: BlendFunctions::BlendDifference(dstColor, srcColor, opacity); break;
						case BlendMode::Exclusion: BlendFunctions::BlendExclusion(dstColor, srcColor, opacity); break;
						case BlendMode::Hue: BlendFunctions::BlendHue(dstColor, srcColor, opacity); break;
						case BlendMode::Saturation: BlendFunctions::BlendSaturation(dstColor, srcColor, opacity); break;
						case BlendMode::Color: BlendFunctions::BlendColor(dstColor, srcColor, opacity); break;
						case BlendMode::Luminosity: BlendFunctions::BlendLuminosity(dstColor, srcColor, opacity); break;
						case BlendMode::Addition: BlendFunctions::BlendAddition(dstColor, srcColor, opacity); break;
						case BlendMode::Subtract: BlendFunctions::BlendSubtract(dstColor, srcColor, opacity); break;
						case BlendMode::Divide: BlendFunctions::BlendDivide(dstColor, srcColor, opacity); break;
						default: BlendFunctions::BlendNormal(dstColor, srcColor, opacity); break;
					}

				}
			}
		}
	}
}

