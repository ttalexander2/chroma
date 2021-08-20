#pragma once

#include <filesystem>

namespace Polychrome
{
	class AssetBrowser
	{
	public:
		static bool Open;
		static std::filesystem::path Selected;
		static void Draw();
		static void HandleOpen(std::filesystem::path path);
	private:
		static void ParseFolder(std::filesystem::path path);
	};
}