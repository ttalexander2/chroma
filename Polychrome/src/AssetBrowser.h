#pragma once

#include <filesystem>
#include <Chroma/Math/Math.h>
#include "Widgets/TextEditor.h"

namespace Polychrome
{
	class AssetBrowser
	{
	public:
		static bool Open;
		static std::filesystem::path Selected;
		static void Draw();
		static void HandleOpen(std::filesystem::path path, TextEditor::Coordinates cursor_pos = TextEditor::Coordinates(1,0));
	private:
		static void ParseFolder(std::filesystem::path path);
	};
}