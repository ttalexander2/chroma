#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/common.hpp>


//Addapted from https://github.com/ocornut/imgui/issues/1901

namespace ImGui
{

    bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);

    bool Spinner(const char* label, float radius, int thickness, const ImU32& color);
    
}