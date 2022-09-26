#include "chromapch.h"
#include "File.h"
#include <physfs.h>
#include "FileSystem.h"

namespace Chroma
{
	File File::Open(const std::string &path, FileMode mode)
	{
		return FileSystem::Open(path, mode);
	}

	File::~File()
	{
		Close();
	}

	bool File::Good()
	{
		return m_Handle != nullptr;
	}

	void File::Close()
	{
		if (!Good())
			return;
		PHYSFS_close(m_Handle);
	}

	bool File::EoF()
	{
		if (!Good())
			return true;

		return PHYSFS_eof(m_Handle) != 0;
	}

	int64_t File::Length()
	{
		if (!Good())
			return -1;
		return PHYSFS_fileLength(m_Handle);
	}

	void File::Flush()
	{
		if (Good())
			PHYSFS_flush(m_Handle);
	}

	int64_t File::Tell()
	{
		if (!Good())
			return -1;

		return PHYSFS_tell(m_Handle);
	}

	bool File::Seek(int64_t pos)
	{
		return PHYSFS_seek(m_Handle, pos) != 0;
	}


	int64_t File::Write(std::string &string)
	{
		return Write(reinterpret_cast<void *>(string.data()), string.size());
	}

	int64_t File::Write(const char *string)
	{
		size_t len = strlen(string);
		return Write((void *)reinterpret_cast<const void *>(string), len);
	}

	int64_t File::Read(void *buffer, size_t size)
	{
		if (!Good())
			return -1;

		int64_t result = PHYSFS_readBytes(m_Handle, buffer, size);
		return result;
	}

	int64_t File::Write(void *buffer, size_t size)
	{
		if (!Good())
		{
			return -1;
		}

		return PHYSFS_writeBytes(m_Handle, buffer, size);
	}


	std::string File::ReadString()
	{
		if (!Good())
		{
			return "";
		}

		uint64_t length = PHYSFS_fileLength(m_Handle);
		if (length <= 0)
		{
			return "";
		}

		std::vector<uint8_t> buffer;
		buffer.resize(length);

		int64_t result = PHYSFS_readBytes(m_Handle, buffer.data(), length);
		if (length <= 0)
		{
			return "";
		}

		return std::string(buffer.begin(), buffer.end());
	}


	std::string File::ReadLine()
	{
		if (!Good())
			return "";

		std::stringstream ss;

		char c = 0;
		do
		{
			bool endLine = false;
			PHYSFS_readBytes(m_Handle, &c, 1);
			if (c == '\n')
			{
				endLine = true;
			}
			else if (c == '\r')
			{
				char r;
				int64_t pos = PHYSFS_tell(m_Handle);
				int64_t result = PHYSFS_readBytes(m_Handle, &r, 1);
				PHYSFS_seek(m_Handle, pos);

				if (result > 0 && r == '\n')
				{
					endLine = true;
				}
			}
			if (!endLine)
				ss << c;
		} while (PHYSFS_eof(m_Handle) == 0 && c != '\n');

		return ss.str();
	}
}
