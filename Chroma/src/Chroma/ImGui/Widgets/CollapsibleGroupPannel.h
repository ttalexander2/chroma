#pragma once
#include "imgui.h"

namespace ImGui
{
	/// @brief Function to display a colapsible group panel. This function should be used with EndCollapsibleGroupPanel.
	/// @param id ID of the panel.
	/// @param size Size of the panel.
	/// @param open Whether the panel should be rendered open or close.
	/// @return Whether the panel is open.
	bool BeginCollapsibleGroupPanel(ImGuiID id, const ImVec2 &size, bool open);

	/// @brief Function to end rendering of the colapsible group panel. This function should be called after BeginCollapsibleGroupPanel().
	void EndCollapsibleGroupPanel();
}
