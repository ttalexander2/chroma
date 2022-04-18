#include "FileDrop.h"
#include <string>
#include "Chroma.h"

#include <filesystem>

#include "Project.h"

#include "AssetBrowser.h"
#include <Chroma/Assets/AssetManager.h>
#include <Chroma/Assets/FMODBank.h>

namespace Polychrome
{
#ifdef CHROMA_PLATFORM_WINDOWS
	void FileDrop::HandleFileDrop(GLFWwindow* window, int count, const char** paths)
	{
		for (int i = 0; i < count; i++)
		{
			std::string path = paths[i];
			if (std::filesystem::exists(AssetBrowser::GetActiveDirectory()) && std::filesystem::exists(path))
			{
				std::filesystem::path newPath = AssetBrowser::GetActiveDirectory();
				newPath /= std::filesystem::path(path).filename();
				std::filesystem::copy_file(path, newPath);
			}
			
		}

		
	}
#endif
}

