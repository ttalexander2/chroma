#pragma once

#include <filesystem>
#include <Chroma/Math/Math.h>
#include <Chroma/Renderer/Texture.h>
#include <Chroma/Core/Core.h>
#include <Chroma/Assets/Asset.h>
#include <Chroma/Utilities/StringHash.h>
#include <map>

namespace Polychrome
{
	class AssetBrowser
	{
	public:
		static bool Open;
		static bool OpenScriptCreatePopup;
		static std::filesystem::path Selected;
		static void Draw();
		static void DrawScriptCreateWindow();
		static void HandleOpen(std::filesystem::path path);
		static bool IsSelecting() { return s_IsSelecting; }

		static std::string GetActiveDirectory();

		template <typename T>
		static void SelectAsset(std::function<void(Chroma::Ref<Chroma::Asset>, void*)> callback, void* user_data = nullptr)
		{
			static_assert(std::is_base_of_v<Chroma::Asset, T>, "Type T is not derived from Asset!");

			s_IsSelecting = true;
			s_SelectionCallback = callback;
			s_SelectionType = T::GetTypeStatic();
			s_UserData = user_data;
		}

		static std::map<std::filesystem::path, Chroma::Ref<Chroma::Texture2D>> Icons;
		static void LoadFileIcon(std::filesystem::path path);

	private:
		static bool ParseFolder(std::filesystem::path path);
		static void FileClickPopup();
		static void FolderClickPopup();
		
		static void AssetBrowserCreatePopup();
		//static std::map<std::filesystem::path, Chroma::Ref<Chroma::Texture2D>> Icons;


		static bool s_IsSelecting;
		static std::function<void(Chroma::Ref<Chroma::Asset>, void*)> s_SelectionCallback;
		static StringHash s_SelectionType;
		static void* s_UserData;
	};
}