#pragma once
#include "imgui.h"

namespace ImGui
{
	/// @brief Displays a group panel. Should be used with EndGroupPanel().
	/// @param name Label for the group panel/
	/// @param size Size of the group panel.
	void BeginGroupPanel(const char* name, const ImVec2& size);

	/// @brief Renders/finishes a group panel. Should be used with BeginGroupPanel().
	void EndGroupPanel();
}