#pragma once

#include "imgui.h"
#include <Chroma/ImGui/IconsForkAwesome.h>

namespace ImGui
{
	/// @brief Function to display a checkbox with alternate icons.
	/// @param active Whether the check box is active.
	inline void AlternateCheckBox(bool *active)
	{
		PushID(active);
		const char *icon;
		if (*active)
		{
			icon = ICON_FK_CHECK_SQUARE_O;
		}
		else
		{
			icon = ICON_FK_SQUARE_O;
		}

		PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		PushStyleColor(ImGuiCol_ButtonActive, { 0, 0, 0, 0 });
		if (Button(icon))
			*active = !*active;
		PopStyleColor(2);
		PopID();
	}
}
