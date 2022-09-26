#include "chromapch.h"
#include "Path.h"

#include "FileSystem.h"

namespace Chroma
{
	static std::string to_lower(const std::string &val)
	{
		std::string temp;
		std::transform(temp.begin(),
				temp.end(),
				temp.begin(),
				[](unsigned char c)
				{
					return std::tolower(c);
				});
		return temp;
	}

	Path::Path(const std::string &path):
		m_Path(path)
	{
		size_t pos = 0;
		if ((pos = m_Path.find(':')) != std::string::npos)
		{
			m_Path = m_Path.substr(pos + 1, m_Path.size() - pos);
		}

		std::replace(m_Path.begin(), m_Path.end(), '\\', '/');
	}

	void Path::operator/=(const Path &path)
	{
		m_Path = m_Path + '/' + path.m_Path;
	}

	void Path::operator+=(const Path &path)
	{
		m_Path = m_Path + path.m_Path;
	}

	Path Path::ParentPath()
	{
		if (m_Path.find('/') == std::string::npos)
			return Path("");
		size_t pos = m_Path.find_last_of('/');
		return Path(m_Path.substr(0, pos));
	}

	std::string Path::Extension()
	{
		if (m_Path.find('.') == std::string::npos)
			return "";
		size_t pos = m_Path.find_last_of('.');
		return m_Path.substr(pos, m_Path.size() - pos);
	}

	std::string Path::Stem()
	{
		size_t pos = m_Path.find_last_of('/');
		return m_Path.substr(pos + 1, m_Path.size() - pos - 1);
	}

	std::string Path::FileName()
	{
		size_t pos = m_Path.find_last_of('/');
		size_t ext_pos = m_Path.find_last_of('.');

		return m_Path.substr(pos + 1, ext_pos - pos - 1);
	}

	bool Path::Empty()
	{
		return m_Path.empty();
	}

	bool Path::Exists()
	{
		return FileSystem::Exists(m_Path);
	}

	bool Path::IsFile()
	{
		return FileSystem::IsFile(m_Path);
	}

	bool Path::IsDirectory()
	{
		return FileSystem::IsDirectory(m_Path);
	}

	bool Path::HasExtension(const std::string &extension)
	{
		return to_lower(Extension()).compare(to_lower(extension)) == 0;
	}
}
