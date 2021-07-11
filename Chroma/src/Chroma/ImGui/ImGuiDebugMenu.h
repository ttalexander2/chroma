#pragma once
#include <vector>
#include <string>

namespace Chroma
{
	class ImGuiDebugMenu
	{
	public:
		static void ShowValue(std::string label, std::string value);
		static void ShowValue(std::string label, bool value);
		static void ShowValue(std::string label, int value);
		static void ShowValue(std::string label, float value);
		static void ShowValue(std::string label, double value);
		static void Draw();


	};
}

#ifndef CHROMA_DEBUG_VALUE
	#define CHROMA_DEBUG_VALUE(label, value) Chroma::ImGuiDebugMenu::ShowValue(label, value)
#endif