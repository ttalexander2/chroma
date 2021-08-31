#pragma once
#include <Chroma/Math/Vec3.h>
#include <Chroma/Math/Vec4.h>

//Adapted from https://github.com/NoelFB/blah/blob/master/include/blah/numerics/color.h

namespace Chroma
{
#define HEX_VALUE(n) ((n >= '0' && n <= '9') ? (n - '0') : ((n >= 'A' && n <= 'F') ? (10 + n - 'A') : ((n >= 'a' && n <= 'f') ? (10 + n - 'a') : 0)))

	/// @brief Simple RGBA color.
	struct Color
	{
		uint8_t r; 		/// @brief Red color channel.
		uint8_t g;		/// @brief Green color channel.
		uint8_t b;		/// @brief Blue color channel.
		uint8_t a;		/// @brief Alpha color channel.

		/// @brief Constructs an RGBA color with all channels set to 0.
		constexpr Color()
			: r(0), g(0), b(0), a(0)
		{
		}

		/// @brief Constructs an RGBA color from an RGB int, with the alpha channel set to 255.
		/// @param rgb 8-bit RGB color, each channel containing 8 bits.
		constexpr Color(int rgb)
			: r((uint8_t)((rgb & 0xFF0000) >> 16))
			, g((uint8_t)((rgb & 0x00FF00) >> 8))
			, b((uint8_t)(rgb & 0x0000FF))
			, a(255)
		{
		}

		/// @brief Constructs an RGBA color from an RGB int, with the alpha channel set to the provided alpha.
		/// @param rgb 8-bit RGB color, each channel containing 8 bits.
		/// @param alpha Alpha amount, from 0-1.
		constexpr Color(int rgb, float alpha)
			: r((int)(((uint8_t)((rgb & 0xFF0000) >> 16))* alpha))
			, g((int)(((uint8_t)((rgb & 0x00FF00) >> 8))* alpha))
			, b((int)(((uint8_t)(rgb & 0x0000FF))* alpha))
			, a((int)(255 * alpha))
		{
		}

		/// @brief Constructs an RGBA color from three 8-bit color channels, with the alpha channel set to 255.
		/// @param r 8-bit Red channel.
		/// @param g 8-bit Green channel.
		/// @param b 8-bit Blue channel.
		constexpr Color(uint8_t r, uint8_t g, uint8_t b)
			: r(r)
			, g(g)
			, b(b)
			, a(255)
		{
		}

		/// @brief Constructs an RGBA color from four 8-bit color channels.
		/// @param r 8-bit Red channel.
		/// @param g 8-bit Green channel.
		/// @param b 8-bit Blue channel.
		/// @param b 8-bit Alpha channel.
		constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			: r(r)
			, g(g)
			, b(b)
			, a(a)
		{
		}

		/// @brief Constructs an RGBA color from a 3 component vector. Sets alpha to 255.
		/// @param vec3 Vector containing red, green, and blue channels.
		constexpr Color(const Math::vec3& vec3)
			: r((int)(vec3.x * 255))
			, g((int)(vec3.y * 255))
			, b((int)(vec3.z * 255))
			, a((int)(255))
		{
		}

		/// @brief Constructs an RGBA color from a 3 component vector. Sets alpha to provided value.
		/// @param vec3 Vector containing red, green, and blue channels in a range from 0 to 1.
		/// @param alpha Alpha amount, from 0-1.
		constexpr Color(const Math::vec3& vec3, float alpha)
			: r((int)(vec3.x* alpha * 255))
			, g((int)(vec3.y* alpha * 255))
			, b((int)(vec3.z* alpha * 255))
			, a((int)(alpha * 255))
		{
		}

		/// @brief Constructs an RGBA color from a 4 component vector.
		/// @param vec3 Vector containing red, green, blue, and alpha channels in a range from 0 to 1.
		constexpr Color(const Math::vec4& vec4)
			: r((int)(vec4.x * 255))
			, g((int)(vec4.y * 255))
			, b((int)(vec4.z * 255))
			, a((int)(vec4.w * 255))
		{
		}

		/// @brief Constructs an RGBA color from a hexidecimal string.
		/// @param hex_string String containing a valid hexidecimal color representation.
		constexpr Color(const char* hex_string)
			: r(0), g(0), b(0), a(255)
		{
			if (*hex_string == '#')
				hex_string += 1;
			else if (*hex_string == '0' && (*(hex_string + 1) == 'x' || *(hex_string + 1) == 'X'))
				hex_string += 2;

			int len = 0;
			while (len < 8 && *(hex_string + len) != '\0')
				len++;

			if (len >= 6)
			{
				r = (HEX_VALUE(hex_string[0]) << 4) + HEX_VALUE(hex_string[1]);
				g = (HEX_VALUE(hex_string[2]) << 4) + HEX_VALUE(hex_string[3]);
				b = (HEX_VALUE(hex_string[4]) << 4) + HEX_VALUE(hex_string[5]);
			}

			if (len >= 8)
				a = (HEX_VALUE(hex_string[6]) << 4) + HEX_VALUE(hex_string[7]);
		}

		/// @brief Premultiply the RGB channels by the alpha value.
		constexpr void PreMultiply()
		{
			r = r * a / 255;
			g = g * a / 255;
			b = b * a / 255;
		}

		/// @brief Converts the color to a RGBA hexidecimal string.
		std::string ToHexRGBA() const;
		/// @brief Converts the color to a RGB hexidecimal string.
		std::string ToHexRGB() const;

		/// @brief Converts the color to a 3 component vector, each channel in a range from 0 - 1.
		const Math::vec3 ToVec3() const
		{
			return Math::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
		}

		/// @brief Converts the color to a 4 component vector, each channel in a range from 0 - 1.
		const Math::vec4 ToVec4() const
		{
			return Math::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		}

		operator Math::vec3() const
		{
			return Math::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
		}

		operator Math::vec4() const
		{
			return Math::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		}

		/// @brief Converts the color to a 32-bit integer, each channel taking 8 bits.
		constexpr uint32_t ToRGBA() const
		{
			return
				((uint32_t)r << 24) |
				((uint32_t)g << 16) |
				((uint32_t)b << 8) |
				(uint32_t)a;
		}

		/// @brief Converts a 32 bit RGBA color integer to a Color.
		static constexpr Color FromRGBA(uint32_t value)
		{
			return
			{
				(uint8_t)((value & 0xFF000000) >> 24),
				(uint8_t)((value & 0x00FF0000) >> 16),
				(uint8_t)((value & 0x0000FF00) >> 8),
				(uint8_t)((value & 0x000000FF))
			};
		}

		/// @brief Converts the color to a 32-bit integer, each channel taking 8 bits.
		constexpr uint32_t ToRGBA()
		{
			return (uint32_t)r << 24 | (uint32_t)g << 16 | (uint32_t)b << 8 | (uint32_t)a;

		}

		/// @brief Converts the color to a 32-bit integer, each channel taking 8 bits.
		static constexpr Color Lerp(Color a, Color b, float amount)
		{
			if (amount < 0) amount = 0;
			if (amount > 1) amount = 1;

			return Color(
				(uint8_t)(a.r + (b.r - a.r) * amount),
				(uint8_t)(a.g + (b.g - a.g) * amount),
				(uint8_t)(a.b + (b.b - a.b) * amount),
				(uint8_t)(a.a + (b.a - a.a) * amount)
			);
		}

		/// @brief Multiply this color by a float value.
		constexpr Color operator*(float multiply) const
		{
			return Color(
				(int)(r * multiply),
				(int)(g * multiply),
				(int)(b * multiply),
				(int)(a * multiply));
		}

		/// @brief Set a color equal to an RGB integer.
		constexpr Color& operator= (int rgb)
		{
			r = (uint8_t)((rgb & 0xFF0000) >> 16);
			g = (uint8_t)((rgb & 0x00FF00) >> 8);
			b = (uint8_t)(rgb & 0x0000FF);
			a = 255;
			return *this;
		}

		/// @brief Multiply this color by a float value.
		constexpr bool operator ==(const Color& rhs) const
		{
			return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
		}

		/// @brief Not equals operator for 2 Colors.
		constexpr bool operator !=(const Color& rhs) const
		{
			return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a;
		}


		static const Color Transparent;
		static const Color White;
		static const Color Black;
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Yellow;
		static const Color Orange;
		static const Color Purple;
		static const Color Teal;

	};

	inline const Color Color::Transparent = Color(0, 0, 0, 0);
	inline const Color Color::White = Color(255, 255, 255, 255);
	inline const Color Color::Black = Color(0, 0, 0, 255);
	inline const Color Color::Red = Color(255, 0, 0, 255);
	inline const Color Color::Green = Color(0, 255, 0, 255);
	inline const Color Color::Blue = Color(0, 0, 255, 255);
	inline const Color Color::Yellow = Color(255, 255, 0, 255);
	inline const Color Color::Purple = Color(255, 0, 255, 255);
	inline const Color Color::Teal = Color(0, 255, 255, 255);
}

#undef HEX_VALUE



