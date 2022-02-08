#pragma once
#include <string>

namespace Polychrome
{
	class Project
	{
	public:
		static bool CreateProject(const std::string& name, const std::string& directory, const std::string& starting_scene);
		static bool LoadProject(const std::string& path);
		static bool SaveCurrentProject();

		static std::string Name;
		static std::string Path;
		static std::string StartingScene;
		static std::string AssetDirectory;

	};
}