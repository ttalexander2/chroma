#pragma once
#include <string>

namespace Polychrome
{
	class Project
	{
	public:
		static bool CreateProject(const std::string& name, const std::string& directory);
		static bool LoadProject(const std::string& path);

		static std::string Name;
		static std::string Path;
	};
}