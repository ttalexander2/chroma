#pragma once

#include <Chroma/Math/Vec4.h>
#include <Chroma/Math/Vec2.h>
#include <Chroma/Images/Color.h>
#include <Chroma/Images/Image.h>
#include <Chroma/IO/Endian.h>

#include <fstream>

namespace Chroma
{
	// Adapted from https://github.com/NoelFB/blah/

	/// @brief Helper function to read data from stream.
	/// @tparam T Type of data to read from stream. T must be arithmetic.
	/// @param stream Stream to read from.
	/// @param endian Endianness the data should be.
	/// @return Returns object read.
	/// 
	/// Read function will swap endianess if data expected doesn't match system.
	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	T read(std::fstream& stream, Endian endian)
	{
		T result;
		stream.read((char*)&result, sizeof(T));
		if (!Chroma::SystemIsEndian(endian))
			Chroma::SwapEndian(&result);
		return result;
	}

	/// @brief Helper function to read data from stream.
	/// @tparam T Type of data to read from stream. T must be arithmetic.
	/// @param stream Stream to read from.
	/// @return Returns object read
	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	T read(std::fstream& stream)
	{
		T result;
		stream.read((char*)&result, sizeof(T));
		return result;
	}

	/// @brief Aseprite object, holding all data parsed from an `.ase` file.
	///
	/// This class was adapted from <a href="https://github.com/NoelFB/blah/">https://github.com/NoelFB/blah/</a>,
	/// and is based on the Aseprite file specification found <a href="https://github.com/aseprite/aseprite/blob/main/docs/ase-file-specs.md">here</a>.
	class Aseprite
	{
	public:

		/// @brief Rendering mode of the images.
		enum class Modes
		{
			Indexed = 1,
			Grayscale = 2,
			RGBA = 4
		};

		/// @brief Enum representing the chunk type.
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

		/// @brief Enum representing the loop direction of a tag.
		enum class LoopDirections
		{
			Forward = 0,
			Reverse = 1,
			PingPong = 2
		};

		/// @brief Flags controlling specific layers.
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

		/// @brief Type of layer.
		enum class LayerTypes
		{
			Normal = 0,
			Group = 1
		};

		/// @brief Blend mode for a layer.
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

		/// @brief Stored user data.
		struct UserData
		{
			std::string text;
			Color color;
		};

		/// @brief Single cel for an aseprite animation.
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

		/// @brief Single frame of animation.
		struct Frame
		{
			int duration = 0;
			Image image;
			std::vector<Cel> cels;
		};

		/// @brief Layer information.
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

		/// @brief Tag holding selection of frames. 
		struct Tag
		{
			std::string name;
			LoopDirections loops = LoopDirections::Forward;
			int from = 0;
			int to = 0;
			Color color;
			UserData userdata;
		};

		/// @brief Aseprite Slice.
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

		/// @brief Asprite mode
		Modes mode = Modes::RGBA;
		/// @brief Sprite width.
		int width = 0;
		/// @brief Sprite height.
		int height = 0;

		/// @brief List of layers.
		std::vector<Layer> layers;
		/// @brief List of frames.
		std::vector<Frame> frames;
		/// @brief List of tags.
		std::vector<Tag>   tags;
		/// @brief List of slices.
		std::vector<Slice> slices;
		/// @brief List of colors in the palette.
		std::vector<Color> palette;

		/// @brief Construct an empty Aseprite.
		Aseprite();

		/// @brief Load an Aseprite from a `.ase` file from a filepath.
		/// @param path Path to the aseprite file.
		/// 
		/// The user of this constructor should check to make sure the path exists before
		/// constructing an Aseprite object.
		Aseprite(const std::string& path);

		/// @brief Load an Aseprite from a byte stream.
		/// @param stream Stream to the Aseprite data.
		Aseprite(std::fstream& stream);

		/// @brief Construct an Aseprite object from an existing Aseprite object.
		/// @param src Aseprite object to copy.
		Aseprite(const Aseprite& src);

		/// @brief Construct an Aseprite object from an existing Aseprite object.
		/// @param src Aseprite object to copy.
		Aseprite(Aseprite&& src) noexcept;

		/// @brief Construct an Aseprite object from an existing Aseprite object.
		/// @param src Aseprite object to copy.
		/// @returs Reference to the new Aseprite object.
		Aseprite& operator=(const Aseprite& src);

		/// @brief Construct an Aseprite object from an existing Aseprite object.
		/// @param src Aseprite object to copy.
		/// @returs Reference to the new Aseprite object.
		Aseprite& operator=(Aseprite&& src) noexcept;

		/// @brief Destroys an Aseprite instance.
		///
		/// TODO: Should probably double check to make sure that images are succesfully deleted.
		~Aseprite();

		void Dispose();

	private:

		/// @brief User data
		UserData* m_last_userdata = nullptr;

		/// @brief Parse a `.ase` file from a stream.
		/// @param stream Stream to parse.
		void parse(std::fstream& stream);

		/// @brief Parse an aseprite layer chunk from a stream.
		/// @param stream Stream to parse.
		/// @param frame Frame number.
		void parse_layer(std::fstream& stream, int frame);

		/// @brief Parse an aseprite cel chunk from a stream.
		/// @param stream Stream to parse.
		/// @param frame Frame number.
		void parse_cel(std::fstream& stream, int frame, size_t maxPosition);

		/// @brief Parse an aseprite pallete chunk from a stream.
		/// @param stream Stream to parse.
		/// @param frame Frame number.
		void parse_palette(std::fstream& stream, int frame);

		/// @brief Parse an aseprite pallete chunk from a stream.
		/// @param stream Stream to parse.
		/// @param frame Frame number.
		void parse_user_data(std::fstream& stream, int frame);

		/// @brief Parse an aseprite tag chunk from a stream.
		/// @param stream Stream to parse.
		/// @param frame Frame number.
		void parse_tag(std::fstream& stream, int frame);

		/// @brief Parse an aseprite slice chunk from a stream.
		/// @param stream Stream to parse.
		/// @param frame Frame number.
		void parse_slice(std::fstream& stream, int frame);

		/// @brief Render a single cel and blend it with the cells beneath it.
		/// @param stream Stream to parse.
		/// @param frame Frame number.
		void render_cel(Cel* cel, Frame* frame);
	};
}

