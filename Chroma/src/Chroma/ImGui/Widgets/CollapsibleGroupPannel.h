#pragma once
#include "imgui.h"

namespace ImGui
{
	bool BeginCollapsibleGroupPanel(ImGuiID id, const ImVec2& size, bool open);
	void EndCollapsibleGroupPanel();
}

