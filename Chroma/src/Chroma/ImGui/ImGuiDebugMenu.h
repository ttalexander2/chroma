#pragma once
#include <vector>
#include <string>

namespace Chroma
{
	/// @brief Class for an ImGui debug menu.
	class ImGuiDebugMenu
	{
	public:
		/// @brief Show a value on the debug menu.
		/// @param label Label for the value.
		/// @param value Value to display.
		static void ShowValue(std::string label, std::string value);
		/// @brief Show a value on the debug menu.
		/// @param label Label for the value.
		/// @param value Value to display.
		static void ShowValue(std::string label, bool value);
		/// @brief Show a value on the debug menu.
		/// @param label Label for the value.
		/// @param value Value to display.
		static void ShowValue(std::string label, int value);
		/// @brief Show a value on the debug menu.
		/// @param label Label for the value.
		/// @param value Value to display.
		static void ShowValue(std::string label, float value);
		/// @brief Show a value on the debug menu.
		/// @param label Label for the value.
		/// @param value Value to display.
		static void ShowValue(std::string label, double value);

		/// @brief Display the debug menu.
		static void Draw();


	};
}

#ifndef CHROMA_DEBUG_VALUE
	#define CHROMA_DEBUG_VALUE(label, value) Chroma::ImGuiDebugMenu::ShowValue(label, value)
#endif