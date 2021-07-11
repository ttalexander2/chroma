#pragma once
#include "imgui.h"

namespace ImGui
{
	void BeginGroupPanel(const char* name, const ImVec2& size);
	void EndGroupPanel();
}