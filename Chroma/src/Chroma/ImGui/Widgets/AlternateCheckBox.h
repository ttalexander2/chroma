#pragma once

#include "imgui.h"
#include <Chroma/ImGui/IconsForkAwesome.h>

namespace ImGui
{
	inline void AlternateCheckBox(bool* active)
	{
		ImGui::PushID(active);
		const char* icon;
		if (*active)
		{
			icon = ICON_FK_CHECK_SQUARE_O;
		}
		else
		{
			icon = ICON_FK_SQUARE_O;
		}

		ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0,0,0,0 });
		if (ImGui::Button(icon))
			*active = !*active;
		ImGui::PopStyleColor(2);
		ImGui::PopID();
	}
}

