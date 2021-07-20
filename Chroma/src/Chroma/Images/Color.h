#pragma once
#include <Chroma/Math/Vec3.h>
#include <Chroma/Math/Vec4.h>

//Adapted from https://github.com/NoelFB/blah/blob/master/include/blah/numerics/color.h

namespace Chroma
{
#define HEX_VALUE(n) ((n >= '0' && n <= '9') ? (n - '0') : ((n >= 'A' && n <= 'F') ? (10 + n - 'A') : ((n >= 'a' && n <= 'f') ? (10 + n - 'a') : 0)))

	struct Color
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		constexpr Color()
			: r(0), g(0), b(0), a(0)
		{
		}

		constexpr Color(int rgb)
			: r((uint8_t)((rgb & 0xFF0000) >> 16))
			, g((uint8_t)((rgb & 0x00FF00) >> 8))
			, b((uint8_t)(rgb & 0x0000FF))
			, a(255)
		{
		}

		constexpr Color(int rgb, float alpha)
			: r((int)(((uint8_t)((rgb & 0xFF0000) >> 16))* alpha))
			, g((int)(((uint8_t)((rgb & 0x00FF00) >> 8))* alpha))
			, b((int)(((uint8_t)(rgb & 0x0000FF))* alpha))
			, a((int)(255 * alpha))
		{
		}

		constexpr Color(uint8_t r, uint8_t g, uint8_t b)
			: r(r)
			, g(g)
			, b(b)
			, a(255)
		{
		}

		constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			: r(r)
			, g(g)
			, b(b)
			, a(a)
		{
		}

		constexpr Color(const Math::vec3& vec3)
			: r((int)(vec3.x * 255))
			, g((int)(vec3.y * 255))
			, b((int)(vec3.z * 255))
			, a((int)(255))
		{
		}

		constexpr Color(const Math::vec3& vec3, float alpha)
			: r((int)(vec3.x* alpha * 255))
			, g((int)(vec3.y* alpha * 255))
			, b((int)(vec3.z* alpha * 255))
			, a((int)(alpha * 255))
		{
		}

		constexpr Color(const Math::vec4& vec4)
			: r((int)(vec4.x * 255))
			, g((int)(vec4.y * 255))
			, b((int)(vec4.z * 255))
			, a((int)(vec4.w * 255))
		{
		}

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

		constexpr void PreMultiply()
		{
			r = r * a / 255;
			g = g * a / 255;
			b = b * a / 255;
		}

		std::string ToHexRGBA() const;
		std::string ToHexRGB() const;

		const Math::vec3 ToVec3() const
		{
			return Math::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
		}

		const Math::vec4 ToVec4() const
		{
			return Math::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		}

		constexpr uint32_t ToRGBA() const
		{
			return
				((uint32_t)r << 24) |
				((uint32_t)g << 16) |
				((uint32_t)b << 8) |
				(uint32_t)a;
		}

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

		constexpr uint32_t ToRGBA()
		{
			return (uint32_t)r << 24 | (uint32_t)g << 16 | (uint32_t)b << 8 | (uint32_t)a;

		}

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

		constexpr Color operator*(float multiply) const
		{
			return Color(
				(int)(r * multiply),
				(int)(g * multiply),
				(int)(b * multiply),
				(int)(a * multiply));
		}

		constexpr Color& operator= (int rgb)
		{
			r = (uint8_t)((rgb & 0xFF0000) >> 16);
			g = (uint8_t)((rgb & 0x00FF00) >> 8);
			b = (uint8_t)(rgb & 0x0000FF);
			a = 255;
			return *this;
		}

		constexpr bool operator ==(const Color& rhs) const
		{
			return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
		}

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



