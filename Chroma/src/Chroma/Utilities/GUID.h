#pragma once
#include <string>

namespace Chroma
{
	struct GUID
	{
		uint32_t Data1;
		uint16_t Data2;
		uint16_t Data3;
		uint8_t  Data4[8];

		static GUID CreateGUID();
		static GUID Parse(const std::string& guid);
		static GUID Zero();

		std::string ToString();

		bool operator==(const GUID& other) const;
		bool operator< (GUID& other) const;



		~GUID() = default;

	private:

		GUID() = default;
	};


}


