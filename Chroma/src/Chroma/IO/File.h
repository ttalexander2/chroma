#pragma once

#include <string>
#include <vector>
#include <type_traits>
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
		static File Open(const std::string &path, FileMode mode = FileMode::Read);

		virtual ~File();

		bool Good();
		void Close();
		bool EoF();
		int64_t Length();
		void Flush();
		int64_t Tell();
		bool Seek(int64_t pos);

		template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>, T>>
		T Read()
		{
			return Read<T>(Endian::Little);
		}

		template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>, T>>
		T Read(Endian endian)
		{
			T result{};
			Read(&result, sizeof(T) / sizeof(char));
			if (!SystemIsEndian(endian))
				Chroma::SwapEndian(&result);
			return result;
		}

		template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>, T>>
		int64_t Read(T *buffer, size_t count)
		{
			return Read(reinterpret_cast<void *>(buffer), count);
		}

		int64_t Read(unsigned char *buffer, size_t count)
		{
			return Read(reinterpret_cast<void *>(buffer), count);
		}

		template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>, T>>
		int64_t Read(std::vector<T> &buffer)
		{
			return Read(reinterpret_cast<void *>(buffer.data()), buffer.size());
		}

		std::string ReadString();
		std::string ReadLine();

		template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>, T>>
		int64_t Write(T *buffer, size_t count)
		{
			return Write(reinterpret_cast<void *>(buffer), count);
		}

		template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>, T>>
		int64_t Write(std::vector<T> &buffer)
		{
			return Write(reinterpret_cast<void *>(buffer.data()), buffer.size());
		}

		template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>, T>>
		int64_t Write(T val)
		{
			return Write(reinterpret_cast<void *>(&val), sizeof(T));
		}

		int64_t Write(std::string &string);
		int64_t Write(const char *string);

		const std::string &GetPath() const { return m_Path; }

	protected:
		File(PHYSFS_File *handle) :
			m_Handle(handle)
		{
		}

		int64_t Read(void *buffer, size_t size);
		int64_t Write(void *buffer, size_t size);


		PHYSFS_File *m_Handle = nullptr;

		std::string m_Path;

		friend class FileSystem;
	};
}
