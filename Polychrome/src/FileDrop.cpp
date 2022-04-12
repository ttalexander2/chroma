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
				std::string relativePath = std::filesystem::relative(newPath, std::filesystem::path(Project::AssetDirectory)).string();
				std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
				if (newPath.extension() == ".png" || newPath.extension() == ".ase" || newPath.extension() == ".aseprite")
				{
					Chroma::Ref<Chroma::Asset> asset = Chroma::AssetManager::Create(Chroma::GUID::CreateGUID(), relativePath, Chroma::Sprite::GetTypeNameStatic());
				}
				else if (newPath.extension() == ".bank")
				{
					Chroma::Ref<Chroma::Asset> asset = Chroma::AssetManager::Create(Chroma::GUID::CreateGUID(), relativePath, Chroma::FMODBank::GetTypeNameStatic());
				}
			}
			
		}

		std::string manifestPath = Project::Path + "//" + "Assets.yaml";

		std::ofstream stream;
		stream.open(manifestPath, std::ios::out | std::ios::trunc);

		stream << Chroma::AssetManager::SaveManifest();
		stream.close();

		
	}
#endif
}

