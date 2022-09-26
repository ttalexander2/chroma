#pragma once
#include <string>

namespace Chroma
{
	struct Path
	{
		Path() = default;
		Path(const Path &) = default;
		Path(Path &&) = default;
		~Path() = default;
		Path &operator=(const Path &) = default;
		Path &operator=(Path &&) = default;

		Path(const std::string &path);


		void operator/=(const Path &path);
		void operator+=(const Path &path);
		operator std::string() { return m_Path; }
		std::string String() { return m_Path; }
		operator const char *() { return m_Path.c_str(); }

		//Decomposition
		Path ParentPath();
		std::string Extension();
		std::string FileName();
		std::string Stem();

		//Queries
		bool Empty();
		bool Exists();
		bool IsFile();
		bool IsDirectory();
		bool HasExtension(const std::string &extension);


	private:
		std::string m_Path;
	};
}
