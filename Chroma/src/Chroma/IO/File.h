#pragma once

#include <Chroma/IO/Endian.h>

struct PHYSFS_File;

namespace Chroma
{
	class FileSystem;

	enum class FileMode
	{
		Append,
		Write,
		Read
	};

	struct File
	{
		static File Open(const std::string& path, FileMode mode = FileMode::Read);

		virtual ~File();

		bool Good();
		void Close();
		bool EoF();
		int64_t Length();
		void Flush();
		int64_t Tell();
		bool Seek(uint64_t pos);

		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		T Read()
		{
			return Read<T>(Endian::Little);
		}

		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		T Read(Endian endian)
		{
			T result;
			Read(&result, sizeof(T));
			if (!Chroma::SystemIsEndian(endian))
				Chroma::SwapEndian(&result);
			return result;
		}

		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		inline int Read(T* buffer, size_t count)
		{
			return Read(reinterpret_cast<void*>(buffer), count * sizeof(T));
		}

		inline int Read(unsigned char* buffer, size_t count)
		{
			return Read(reinterpret_cast<void*>(buffer), count);
		}

		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		inline int Read(std::vector<T>& buffer)
		{
			return Read(reinterpret_cast<void*>(buffer.data()), buffer.size() * sizeof(T));
		}

		std::string ReadString();
		std::string ReadLine();

		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		int Write(T* buffer, size_t count)
		{
			return Write(reinterpret_cast<void*>(buffer), count * sizeof(T));
		}

		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		int Write(std::vector<T>& buffer)
		{
			return Write(reinterpret_cast<void*>(buffer.data()), buffer.size() * sizeof(T));
		}

		int Write(std::string& string);
		int Write(const char* string);
		
	protected:
		File(PHYSFS_File* handle) : m_Handle(handle) {}

		int Read(void* buffer, size_t size);
		int Write(void* buffer, size_t size);


		PHYSFS_File* m_Handle = NULL;

		friend class FileSystem;
	};
}