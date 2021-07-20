#include "chromapch.h"
#include "Guid.h"
#include <random>
#include <iomanip>
#include <iostream>
#include <regex>

namespace Chroma
{


	unsigned int RandomUnsignedChar()
	{
		std::random_device random;
		std::mt19937 gen(random());
		std::uniform_int_distribution<> dis(0, 255);
		return dis(gen);
	}

	GUID GUID::CreateGUID()
	{
		GUID result;
		uint8_t vals[4] = { RandomUnsignedChar(), RandomUnsignedChar(), RandomUnsignedChar(), RandomUnsignedChar() };
		result.Data1 = *(uint32_t*)vals;
		uint8_t vals2[2] = { RandomUnsignedChar(), RandomUnsignedChar() };
		result.Data2 = *(uint16_t*)vals2;
		uint8_t vals3[2] = { RandomUnsignedChar(), RandomUnsignedChar() };
		result.Data3 = *(uint16_t*)vals3;
		for (int i = 0; i < 8; i++)
		{
			result.Data4[i] = RandomUnsignedChar();
		}
		return result;
	}

	GUID GUID::Parse(const std::string& guid)
	{
		return GUID();
	}

	GUID GUID::Zero()
	{
		return GUID{};
	}

	std::string GUID::ToString()
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
			stream << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)Data4[i];
			result += stream.str();
		}

		return result;
	}

	bool GUID::operator==(const GUID& other) const
	{
		return memcmp(this, &other, sizeof(other)) == 0;
	}

	bool GUID::operator<(GUID& other) const
	{
		return memcmp(this, &other, sizeof(other)) < 0;
	}

}