#include "chromapch.h"
#include "Aseprite.h"

#define STBI_NO_STDIO
#define STBI_ONLY_ZLIB
#include "stb_image.h"

#include "BlendFunctions.h"
#include "Chroma/IO/File.h"
#include "Chroma/IO/FileSystem.h"



namespace Chroma
{


	Aseprite::Aseprite()
		= default;

	Aseprite::Aseprite(const std::string& path)
	{
		Chroma::File fs = FileSystem::Open(path, FileMode::Read);
		parse(fs);
		fs.Close();
	}

	Aseprite::Aseprite(Chroma::File& stream)
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

	void Aseprite::parse(Chroma::File& stream)
	{
		if (!stream.Good())
		{
			CHROMA_CORE_ASSERT(false, "std::fstream is not readable");
			return;
		}

		int frame_count = 0;

		// header
		{
			// filesize
			uint64_t first = stream.Read<uint32_t>(Endian::Little);

			// magic number
			uint16_t magic = stream.Read<uint16_t>(Endian::Little);
			if (magic != 0xA5E0)
			{
				CHROMA_CORE_ASSERT(false, "File is not a valid Aseprite file");
				return;
			}

			// main info
			frame_count = stream.Read<uint16_t>(Endian::Little);
			width = stream.Read<uint16_t>(Endian::Little);
			height = stream.Read<uint16_t>(Endian::Little);
			mode = static_cast<Aseprite::Modes>(stream.Read<uint16_t>(Endian::Little) / 8);

			// don't care about other info
			stream.Read<uint32_t>(Endian::Little);				// Flags
			stream.Read<uint16_t>(Endian::Little);				// Speed (deprecated)
			stream.Read<uint32_t>(Endian::Little);				// Should be 0
			stream.Read<uint32_t>(Endian::Little);				// Should be 0
			stream.Read<uint8_t>(Endian::Little);				// Palette entry
			stream.Seek(3 + stream.Tell());						// Ignore these bytes
			stream.Read<uint16_t>(Endian::Little);				// Number of colors (0 means 256 for old sprites)
			stream.Read<int8_t>(Endian::Little);				// Pixel width
			stream.Read<int8_t>(Endian::Little);				// Pixel height
			stream.Seek(92 + stream.Tell());					// For Future
		}

		frames.resize(frame_count);

		//CHROMA_CORE_TRACE("Frame Count: {0}", frame_count);

		// frames
		for (int i = 0; i < frame_count; i++)
		{
			auto frameStart = stream.Tell();

			auto offset = stream.Read<uint32_t>(Endian::Little);

			stream.Seek((int64_t)offset - 4 + stream.Tell());
			auto frameEnd = stream.Tell();

			stream.Seek(frameStart);

			stream.Seek(4 + stream.Tell());

			unsigned int chunks = 0;

			// frame header
			{
				auto magic = stream.Read<uint16_t>(Endian::Little); // magic number
				if (magic != 0xF1FA)
				{
					CHROMA_CORE_ASSERT(false, "File is not a valid Aseprite file! Magic: {:x}", magic);
					return;
				}

				auto old_chunk_count = stream.Read<uint16_t>(Endian::Little);
				frames[i].duration = stream.Read<uint16_t>(Endian::Little);
				stream.Seek(2 + stream.Tell());			// for future
				auto new_chunk_count = stream.Read<uint32_t>(Endian::Little);

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
				auto chunkStart = stream.Tell();
				uint32_t chunkSize = stream.Read<uint32_t>(Endian::Little);
				stream.Seek((int64_t)chunkSize - 4 + stream.Tell());
				auto chunkEnd = stream.Tell();
				stream.Seek(chunkStart);
				stream.Seek(4 + stream.Tell());

				auto chunkType = static_cast<Chunks>(stream.Read<uint16_t>(Endian::Little));

				//CHROMA_CORE_TRACE("Chunk [{}]: Start[{}], End[{}], Size[{}], Type[0x{:x}]", j, (int64_t)chunkStart, (int64_t)chunkEnd, chunkSize, chunkType);



				switch (chunkType)
				{
					case Chunks::Layer: parse_layer(stream, i); break;
					case Chunks::Cel: parse_cel(stream, i, (int64_t)chunkEnd); break;
					case Chunks::Palette: parse_palette(stream, i); break;
					case Chunks::UserData: parse_user_data(stream, i); break;
					case Chunks::FrameTags: parse_tag(stream, i); break;
					case Chunks::Slice: parse_slice(stream, i); break;
					default: break;
				}

				stream.Seek(chunkEnd);
			}

			stream.Seek(frameEnd);
		}
	}

	void Aseprite::parse_layer(File& stream, int frame)
	{
		layers.emplace_back();

		auto& layer = layers.back();
		layer.flag = static_cast<LayerFlags>(stream.Read<uint16_t>(Endian::Little));
		layer.visible = ((int)layer.flag & (int)LayerFlags::Visible) == (int)LayerFlags::Visible;
		layer.type = static_cast<LayerTypes>(stream.Read<uint16_t>(Endian::Little));
		layer.child_level = stream.Read<uint16_t>(Endian::Little);
		stream.Read<uint16_t>(Endian::Little); // width
		stream.Read<uint16_t>(Endian::Little); // height
		int blendMode = stream.Read<uint16_t>(Endian::Little);
		if (blendMode < 0 || blendMode > 18)
			blendMode = 0;
		layer.blendmode = static_cast<BlendMode>(blendMode);
		layer.alpha = stream.Read<uint8_t>(Endian::Little);
		stream.Seek(3+ stream.Tell()); // for future

		char* buff = new char[stream.Read<uint16_t>(Endian::Little)];
		stream.Read(buff, layer.name.length());
		layer.name = std::string(buff);
		delete[] buff;

		layer.userdata.color = {255, 255, 255, 1};
		layer.userdata.text = "";
		m_last_userdata = &(layer.userdata);
	}

	void Aseprite::parse_cel(File& stream, int frameIndex, size_t maxPosition)
	{
		Frame& frame = frames[frameIndex];

		frame.cels.emplace_back();
		auto& cel = frame.cels.back();
		cel.layer_index = stream.Read<uint16_t>(Endian::Little);
		cel.x = stream.Read<uint16_t>(Endian::Little);
		cel.y = stream.Read<uint16_t>(Endian::Little);
		cel.alpha = stream.Read<uint8_t>(Endian::Little);
		cel.linked_frame_index = -1;

		auto celType = stream.Read<uint16_t>(Endian::Little);
		stream.Seek(7 + stream.Tell());

		// RAW or DEFLATE
		if (celType == 0 || celType == 2)
		{
			auto width = stream.Read<uint16_t>(Endian::Little);
			auto height = stream.Read<uint16_t>(Endian::Little);
			auto count = width * height * (int)mode;

			cel.image = Image(width, height);

			// RAW
			if (celType == 0)
			{
				stream.Read(reinterpret_cast<char*>(cel.image.Pixels), count);
				//stream.read(cel.image.Pixels, count);
			}
			// DEFLATE (zlib)
			else
			{
				// this could be optimized to use a buffer on the stack if we only read set chunks at a time
				// stbi's zlib doesn't have that functionality though
				auto size = maxPosition - (int64_t)stream.Tell();
				if (size > INT32_MAX)
					size = INT32_MAX;

				char* buffer = new char[size];
				stream.Read(buffer, size);

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
			cel.linked_frame_index = stream.Read<uint16_t>(Endian::Little);
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

	void Aseprite::parse_palette(File& stream, int frame)
	{
		/* size */ stream.Read<uint32_t>(Endian::Little);
		auto start = stream.Read<uint32_t>(Endian::Little);
		auto end = stream.Read<uint32_t>(Endian::Little);
		stream.Seek(8 + stream.Tell());

		palette.resize(palette.size() + (end - start + 1));

		for (int p = 0, len = static_cast<int>(end - start) + 1; p < len; p++)
		{
			auto hasName = stream.Read<uint16_t>(Endian::Little);
			palette[start + p] = stream.Read<uint32_t>(Endian::Little);

			if (hasName & 0xF000)
			{
				int len = stream.Read<uint16_t>(Endian::Little);
				stream.Seek(len + stream.Tell());
			}
		}
	}

	void Aseprite::parse_user_data(File& stream, int frame)
	{
		if (m_last_userdata != nullptr)
		{
			auto flags = stream.Read<uint32_t>(Endian::Little);

			// has text
			if (flags & (1 << 0))
			{
				char* buff = new char[stream.Read<uint16_t>(Endian::Little)];
				stream.Read(buff, m_last_userdata->text.length());
				m_last_userdata->text = std::string(buff);
				delete[] buff;
			}

			// has color
			if (flags & (1 << 1))
				m_last_userdata->color = stream.Read<uint32_t>(Endian::Little);
		}
	}

	void Aseprite::parse_tag(File& stream, int frame)
	{
		auto count = stream.Read<uint16_t>(Endian::Little);
		stream.Seek(8 + stream.Tell());

		for (int t = 0; t < count; t++)
		{
			Tag tag;
			tag.from = stream.Read<uint16_t>(Endian::Little);
			tag.to = stream.Read<uint16_t>(Endian::Little);
			tag.loops = static_cast<LoopDirections>(stream.Read<int8_t>(Endian::Little));

			stream.Seek(8 + stream.Tell());
			tag.color = Color(stream.Read<int8_t>(), stream.Read<int8_t>(), stream.Read<int8_t>(Endian::Little), 255);
			stream.Seek(1 + stream.Tell());

			uint16_t len = stream.Read<uint16_t>(Endian::Little);
			char* buff = new char[len];
			stream.Read(buff, len);
			tag.name = std::string(buff, buff + len);
			delete[] buff;

			tags.push_back(tag);
		}
	}

	void Aseprite::parse_slice(File& stream, int frame)
	{
		int count = stream.Read<uint32_t>(Endian::Little);
		int flags = stream.Read<uint32_t>(Endian::Little);
		stream.Read<uint32_t>(Endian::Little); // reserved

		std::string name;
		char* buff = new char[stream.Read<uint16_t>(Endian::Little)];
		stream.Read(buff, name.length());
		name = std::string(buff);
		delete[] buff;

		for (int s = 0; s < count; s++)
		{
			slices.emplace_back();

			auto& slice = slices.back();
			slice.name = name;
			slice.frame = stream.Read<uint32_t>(Endian::Little);
			slice.origin.x = static_cast<float>(stream.Read<int32_t>(Endian::Little));
			slice.origin.y = static_cast<float>(stream.Read<int32_t>(Endian::Little));
			slice.width = stream.Read<uint32_t>(Endian::Little);
			slice.height = stream.Read<uint32_t>(Endian::Little);

			// 9 slice (ignored atm)
			if (flags & (1 << 0))
			{
				stream.Read<int32_t>(Endian::Little);
				stream.Read<int32_t>(Endian::Little);
				stream.Read<uint32_t>(Endian::Little);
				stream.Read<uint32_t>(Endian::Little);
			}

			// pivot point
			slice.has_pivot = false;
			if (flags & (1 << 1))
			{
				slice.has_pivot = true;
				slice.pivot.x = static_cast<float>(stream.Read<uint32_t>(Endian::Little));
				slice.pivot.y = static_cast<float>(stream.Read<uint32_t>(Endian::Little));
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

