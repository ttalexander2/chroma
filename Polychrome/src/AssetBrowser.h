#pragma once

#include <filesystem>
#include <Chroma/Math/Math.h>
#include <Chroma/Renderer/Texture.h>
#include <map>

namespace Polychrome
{
	class AssetBrowser
	{
	public:
		static bool Open;
		static std::filesystem::path Selected;
		static void Draw();
		static void HandleOpen(std::filesystem::path path);

		static std::map<std::filesystem::path, Chroma::Ref<Chroma::Texture2D>> Icons;
		static void LoadFileIcon(std::filesystem::path path);

	private:
		static bool ParseFolder(std::filesystem::path path);
		static void FileClickPopup();
		static void FolderClickPopup();
		
		static void AssetBrowserCreatePopup();
		//static std::map<std::filesystem::path, Chroma::Ref<Chroma::Texture2D>> Icons;
	};
}