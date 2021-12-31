#include "AssetCreation.h"

#include <filesystem>
#include <fstream>
#include "Project.h"

namespace Polychrome
{

	bool AssetCreation::NewCSharpScript(const std::string& path)
	{
		if (std::filesystem::exists(path))
			return false;

		std::ofstream sc = std::ofstream(path);
		if (!sc.good())
			return false;


	}
}