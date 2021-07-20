#pragma once

#include <Chroma/Math/Vec4.h>
#include <Chroma/Math/Vec2.h>
#include <Chroma/Images/Color.h>
#include <Chroma/Images/Image.h>

#include <fstream>

namespace Chroma
{
	// Adapted from https://github.com/NoelFB/blah/

	enum class Endian
	{
		Little,
		Big
	};

	template<class T>
	inline void swap_endian(T* value)
	{
		for (int i = 0; i < sizeof(T) / 2; i++)
		{
			char* _ptr = (char*)&value;
			char _temp = *(_ptr + i);
			*(_ptr + i) = *(_ptr + sizeof(T) - i - 1);
			*(_ptr + sizeof(T) - i - 1) = _temp;
		}
	}

	inline bool is_big_endian()
	{
		return (*((short*)"AB") == 0x4243);
	}

	inline bool is_little_endian()
	{
		return (*((short*)"AB") != 0x4243);
	}

	inline bool is_endian(const Endian& endian)
	{
		return
			(endian == Endian::Little && is_little_endian()) ||
			(endian == Endian::Big && is_big_endian());
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	T read(std::fstream& stream, Endian endian)
	{
		T result;
		stream.read((char*)&result, sizeof(T));
		if (!is_endian(endian))
			swap_endian(&result);
		return result;
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	T read(std::fstream& stream)
	{
		T result;
		stream.read((char*)&result, sizeof(T));
		return result;
	}

	class Aseprite
	{
	public:

		enum class Modes
		{
			Indexed = 1,
			Grayscale = 2,
			RGBA = 4
		};

		enum class Chunks
		{
			OldPaletteA = 0x0004,
			OldPaletteB = 0x0011,
			Layer = 0x2004,
			Cel = 0x2005,
			CelExtra = 0x2006,
			Mask = 0x2016,
			Path = 0x2017,
			FrameTags = 0x2018,
			Palette = 0x2019,
			UserData = 0x2020,
			Slice = 0x2022
		};

		enum class LoopDirections
		{
			Forward = 0,
			Reverse = 1,
			PingPong = 2
		};

		enum class LayerFlags
		{
			Visible = 1,
			Editable = 2,
			LockMovement = 4,
			Background = 8,
			PreferLinkedCels = 16,
			Collapsed = 32,
			Reference = 64
		};

		enum class LayerTypes
		{
			Normal = 0,
			Group = 1
		};

		enum class BlendMode
		{
			Normal = 0,
			Multiply = 1,
			Screen = 2,
			Overlay = 3,
			Darken = 4,
			Lighten = 5,
			ColorDodge = 6,
			ColorBurn = 7,
			HardLight = 8,
			SoftLight = 9,
			Difference = 10,
			Exclusion = 11,
			Hue = 12,
			Saturation = 13,
			Color = 14,
			Luminosity = 15,
			Addition = 16,
			Subtract = 17,
			Divide = 18
		};

		struct UserData
		{
			std::string text;
			Color color;
		};

		struct Cel
		{
			int layer_index = 0;
			int linked_frame_index = 0;
			int x = 0;
			int y = 0;
			unsigned char alpha = 0;
			Image image;
			UserData userdata;
		};

		struct Frame
		{
			int duration = 0;
			Image image;
			std::vector<Cel> cels;
		};

		struct Layer
		{
			LayerFlags flag = (LayerFlags)0;
			LayerTypes type = LayerTypes::Normal;
			std::string name;
			int child_level = 0;
			BlendMode blendmode = BlendMode::Normal;
			unsigned char alpha = 0;
			bool visible = true;
			UserData userdata;
		};

		struct Tag
		{
			std::string name;
			LoopDirections loops = LoopDirections::Forward;
			int from = 0;
			int to = 0;
			Color color;
			UserData userdata;
		};

		struct Slice
		{
			int frame = 0;
			std::string name;
			Math::vec2 origin;
			int width = 0;
			int height = 0;
			bool has_pivot = false;
			Math::vec2 pivot;
			UserData userdata;
		};

		Modes mode = Modes::RGBA;
		int width = 0;
		int height = 0;

		std::vector<Layer> layers;
		std::vector<Frame> frames;
		std::vector<Tag>   tags;
		std::vector<Slice> slices;
		std::vector<Color> palette;

		Aseprite();
		Aseprite(const std::string& path);
		Aseprite(std::fstream& stream);
		Aseprite(const Aseprite& src);
		Aseprite(Aseprite&& src) noexcept;
		Aseprite& operator=(const Aseprite& src);
		Aseprite& operator=(Aseprite&& src) noexcept;
		~Aseprite();

	private:
		UserData* m_last_userdata = nullptr;

		void parse(std::fstream& stream);
		void parse_layer(std::fstream& stream, int frame);
		void parse_cel(std::fstream& stream, int frame, size_t maxPosition);
		void parse_palette(std::fstream& stream, int frame);
		void parse_user_data(std::fstream& stream, int frame);
		void parse_tag(std::fstream& stream, int frame);
		void parse_slice(std::fstream& stream, int frame);
		void render_cel(Cel* cel, Frame* frame);
	};
}

