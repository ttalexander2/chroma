#include "chromapch.h"
#include "Color.h"

namespace Chroma
{
	std::string Color::ToHexRGBA() const
	{
		static const char* hex = "0123456789ABCDEF";

		std::string str = "00000000";
		str[0] = hex[(r & 0xF0) >> 4];
		str[1] = hex[(r & 0x0F) >> 0];
		str[2] = hex[(g & 0xF0) >> 4];
		str[3] = hex[(g & 0x0F) >> 0];
		str[4] = hex[(b & 0xF0) >> 4];
		str[5] = hex[(b & 0x0F) >> 0];
		str[6] = hex[(a & 0xF0) >> 4];
		str[7] = hex[(a & 0x0F) >> 0];
		return str;
	}
	std::string Color::ToHexRGB() const
	{
		static const char* hex = "0123456789ABCDEF";

		std::string str = "000000";
		str[0] = hex[(r & 0xF0) >> 4];
		str[1] = hex[(r & 0x0F) >> 0];
		str[2] = hex[(g & 0xF0) >> 4];
		str[3] = hex[(g & 0x0F) >> 0];
		str[4] = hex[(b & 0xF0) >> 4];
		str[5] = hex[(b & 0x0F) >> 0];
		return str;
	}
}