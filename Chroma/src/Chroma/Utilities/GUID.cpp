#include "chromapch.h"
#include "Guid.h"
#include <random>
#include <iomanip>
#include <iostream>
#include <regex>
#include <spdlog/fmt/bin_to_hex.h>

namespace Chroma
{
	uint8_t RandomUnsignedChar()
	{
		std::random_device random;
		std::mt19937 gen(random());
		std::uniform_int_distribution<> dis(0, 255);
		return static_cast<uint8_t>(dis(gen));
	}
	
	const GUID GUID::CreateGUID()
	{
		GUID result;
		uint8_t vals[4] = { RandomUnsignedChar(), RandomUnsignedChar(), RandomUnsignedChar(), RandomUnsignedChar() };
		result.Data1 = *(uint32_t *)vals;
		uint8_t vals2[2] = { RandomUnsignedChar(), RandomUnsignedChar() };
		result.Data2 = *(uint16_t *)vals2;
		uint8_t vals3[2] = { RandomUnsignedChar(), RandomUnsignedChar() };
		result.Data3 = *(uint16_t *)vals3;
		for (int i = 0; i < 8; i++)
		{
			result.Data4[i] = RandomUnsignedChar();
		}
		return result;
	}

	const GUID GUID::Parse(const std::string &guid)
	{
		std::string s = guid;

		//convert the string to lowercase
		std::locale loc;
		for (std::string::size_type i = 0; i < s.length(); i++)
			s[i] = std::tolower(s[i], loc);

		//Remove non aphanumeric characters
		std::erase_if(s, [](char c) { return !std::isalnum(c); });

		size_t pos = std::string::npos;

		std::string toErase("0x");

		while ((pos = s.find(toErase)) != std::string::npos)
			s.erase(pos, toErase.length());

		if (s.length() != 32)
			return Zero();

		GUID result{};
		result.Data1 = static_cast<uint32_t>(std::stoll(s.substr(0, 8), nullptr, 16));
		result.Data2 = static_cast<uint32_t>(std::stoll(s.substr(8, 4), nullptr, 16));
		result.Data3 = static_cast<uint32_t>(std::stoll(s.substr(12, 4), nullptr, 16));
		for (size_t i = 0; i < 8; i++)
		{
			result.Data4[i] = static_cast<uint8_t>(std::stoll(s.substr((2 * i) + 16, 2), nullptr, 16));
		}

		return result;
	}

	const GUID GUID::Zero()
	{
		return GUID{};
	}

	std::string GUID::ToString() const
	{
		std::string result;
		std::ostringstream stream;
		stream << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << Data1;
		result = stream.str();
		result += "-";

		stream = std::ostringstream();
		stream << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << Data2;
		result += stream.str();
		result += "-";

		stream = std::ostringstream();
		stream << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << Data3;
		result += stream.str();
		result += "-";
		for (int i = 0; i < 8; i++)
		{
			stream = std::ostringstream();
			stream << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(Data4[i]);
			result += stream.str();
			if (i == 1)
				result += "-";
		}

		return result;
	}

	bool GUID::operator==(const GUID &other) const
	{
		return memcmp(this, &other, sizeof(other)) == 0;
	}

	bool GUID::operator<(GUID &other) const
	{
		return memcmp(this, &other, sizeof(other)) < 0;
	}
}
