#pragma once

#include <imgui.h>
#include <vector>
#include <string>
#include <Chroma/ImGui/IconsForkAwesome.h>
#include <Chroma/ImGui/ImGuiDebugMenu.h>
#include <imgui_stdlib.h>
#include <Chroma/ImGui/Widgets/CollapsibleGroupPannel.h>
#include <imgui_internal.h>
#include <Chroma/ImGui/ImGuiHelper.h>
#include <Chroma/Utilities/ContainerHelpers.h>
#include <unordered_map>

namespace ImGui
{
    static std::unordered_map<ImGuiID, int> active;
    static std::unordered_map<ImGuiID, int> hovered;
    static std::unordered_map<ImGuiID, int> clicked;

	/// @brief Editable list imgui widget.
	/// @param list List of strings to display/edit.
	/// @param open Whether the list is open.
	/// @return Whether the list is open
    /// 
    /// Widget contains list of strings, allows user to edit, reorder, add, and delete strings.
	static bool EditableList(std::vector<std::string> &list, bool open)
	{

        ImGui::PushID(&list);
        ImGuiID id = ImGui::GetID(&list);


        if (active.size() > 100)
            active.clear();
        if (hovered.size() > 100)
            hovered.clear();
        if (clicked.size() > 100)
            clicked.clear();

        if (open)
        {
            if (!active.contains(id))
                active.emplace(id, -1);
            if (!hovered.contains(id))
                hovered.emplace(id, -1);
            if (!clicked.contains(id))
                clicked.emplace(id, 0);
        }


        if (ImGui::BeginCollapsibleGroupPanel(id, ImGui::GetContentRegionAvail(), open))
        {
            bool any_hovered = false;
            bool mouseReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
            for (int i = 0; i < list.size(); i++)
            {
                ImGui::PushID(i);
                std::string item = list[i];

                ImGui::AlignTextToFramePadding();

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

                ImVec4 col = ImGui::GetStyleColorVec4(ImGuiCol_Header);
                ImGui::PushStyleColor(ImGuiCol_Header, { col.x, col.y, col.z, 0.8f });
                if (hovered[id] == i)
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { col.x, col.y, col.z, 0.7f });

                bool is_selected = (i == clicked[id]);
                ImGui::Selectable((std::string("##selectable") + std::to_string(i)).c_str(), &is_selected, ImGuiSelectableFlags_AllowItemOverlap, { ImGui::GetContentRegionAvail().x - 3, 0 });


                if (ImGui::IsItemClicked())
                {
                    clicked[id] = i;
                }

                if (hovered[id] == i)
                    ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                

                if (active[id] == -1 && ImGui::IsItemActive())
                {
                    active[id] = i;
                }
                
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
                {
                    hovered[id] = i;
                    any_hovered = true;
                }

                ImGui::SameLine(-1);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

                if (is_selected)
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0,0,0,0 });
                if (hovered[id] == i && any_hovered)
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0,0,0,0 });

                ImGui::AlignTextToFramePadding();
                ImGui::Text(ICON_FK_BARS);
                
                ImGui::SameLine(24.0f);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                std::string input_hash = std::string("##input_text") + std::to_string(i);

                ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ImGui::GetStyleColorVec4(ImGuiCol_BorderShadow));

                ImGui::InputText(input_hash.c_str(), &list[i]);

                ImGui::PopStyleColor();

                if (is_selected)
                    ImGui::PopStyleColor();
                if (hovered[id] == i && any_hovered)
                    ImGui::PopStyleColor();

                ImGui::PopID();
            }

        }


        if (open)
        {
            if (active[id] != -1 && hovered[id] != -1 && hovered[id] != active[id])
            {
                std::string item = list[active[id]];
                int n_next = active[id] + (ImGui::GetMouseDragDelta(0).y < 0.1f ? -1 : 1);
                if (n_next >= 0 && n_next < list.size())
                {
                    list[active[id]] = list[n_next];
                    list[n_next] = item;
                    ImGui::ResetMouseDragDelta();
                    active[id] = n_next;
                    clicked[id] = n_next;
                }
            }

            if (IsMouseReleased(ImGuiMouseButton_Left))
            {
                active[id] = -1;
            }
        }



        ImGui::EndCollapsibleGroupPanel();

        if (open)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
            ImVec4 c = ImGui::GetStyleColorVec4(ImGuiCol_Border);
            ImGui::PushStyleColor(ImGuiCol_Button, { c.x, c.y, c.z, c.w / 2.5f });

            ImGui::Dummy({ 1.0f, 0.0f });
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 50.0f);


            if (ImGui::Button(ICON_FK_PLUS))
            {
                list.push_back("");
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FK_MINUS))
            {
                if (clicked[id] != -1)
                    Chroma::RemoveAtIndex<std::vector<std::string>>(list, clicked[id]);
                if (clicked[id] > static_cast<int>(list.size()) - 1)
                    clicked[id] = static_cast<int>(list.size()) - 1;
            }


            ImGui::PopStyleColor();
            ImGui::PopStyleVar(2);
        }

        ImGui::PopID();
       
        return open;
	}

    
}