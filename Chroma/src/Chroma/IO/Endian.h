#pragma once

namespace Chroma
{
	enum class Endian
	{
		Little,
		Big
	};

	/// @brief Function to swap endianness of bytes.
	/// @tparam T Type of data.
	/// @param value Value to swap.
	template<class T>
	inline void SwapEndian(T* value)
	{
		for (int i = 0; i < sizeof(T) / 2; i++)
		{
			char* _ptr = (char*)&value;
			char _temp = *(_ptr + i);
			*(_ptr + i) = *(_ptr + sizeof(T) - i - 1);
			*(_ptr + sizeof(T) - i - 1) = _temp;
		}
	}

	inline void SwapEndian(void* value, size_t size)
	{
		for (int i = 0; i < size / 2; i++)
		{
			char* _ptr = (char*)&value;
			char _temp = *(_ptr + i);
			*(_ptr + i) = *(_ptr + size - i - 1);
			*(_ptr + size - i - 1) = _temp;
		}
	}

	/// @brief Checks if system uses big endianness.
	inline bool SystemIsBigEndian()
	{
		return (*((short*)"AB") == 0x4243);
	}

	/// @brief Checks if system uses little endianness.
	inline bool SystemIsLittleEndian()
	{
		return (*((short*)"AB") != 0x4243);
	}

	/// @brief Checks if system is endianess.
	/// @param endian Big or little endian to check.
	/// @return Returns true if endianess matches system.
	inline bool SystemIsEndian(const Endian& endian)
	{
		return
			(endian == Endian::Little && SystemIsLittleEndian()) ||
			(endian == Endian::Big && SystemIsBigEndian());
	}
}