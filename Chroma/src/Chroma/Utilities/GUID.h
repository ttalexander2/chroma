#pragma once
#include <string>
#include <xhash>

namespace Chroma
{
	struct GUID
	{
		uint32_t Data1 = 0;
		uint16_t Data2 = 0;
		uint16_t Data3 = 0;
		uint8_t  Data4[8] = {0,0,0,0,0,0,0,0};

		const static GUID CreateGUID();
		const static GUID Parse(const std::string& guid);
		const static GUID Zero();

		std::string ToString() const;

		bool operator==(const GUID& other) const;
		bool operator< (GUID& other) const;
	};


}

namespace std
{
	template<>
	struct hash<Chroma::GUID>
	{
		std::size_t operator()(const Chroma::GUID& guid) const
		{
			return hash<uint64_t>()((uint64_t)guid.Data1 | (uint64_t)guid.Data2 << 32 | (uint64_t)guid.Data3 << 48);
		}
	};
}


