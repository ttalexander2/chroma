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
		PushID(&list);
		ImGuiID id = GetID(&list);

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

		if (BeginCollapsibleGroupPanel(id, GetContentRegionAvail(), open))
		{
			bool any_hovered = false;
			bool mouseReleased = IsMouseReleased(ImGuiMouseButton_Left);
			for (int i = 0; i < list.size(); i++)
			{
				PushID(i);
				std::string item = list[i];

				AlignTextToFramePadding();

				SetCursorPosX(GetCursorPosX() + 3);

				ImVec4 col = GetStyleColorVec4(ImGuiCol_Header);
				PushStyleColor(ImGuiCol_Header, { col.x, col.y, col.z, 0.8f });
				if (hovered[id] == i)
					PushStyleColor(ImGuiCol_HeaderHovered, { col.x, col.y, col.z, 0.7f });

				bool is_selected = (i == clicked[id]);
				Selectable((std::string("##selectable") + std::to_string(i)).c_str(), &is_selected, ImGuiSelectableFlags_AllowItemOverlap, { GetContentRegionAvail().x - 3, 0 });

				if (IsItemClicked())
				{
					clicked[id] = i;
				}

				if (hovered[id] == i)
					PopStyleColor();
				PopStyleColor();

				if (active[id] == -1 && IsItemActive())
				{
					active[id] = i;
				}

				if (IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
				{
					hovered[id] = i;
					any_hovered = true;
				}

				SameLine(-1);
				SetCursorPosX(GetCursorPosX() + 3);

				if (is_selected)
					PushStyleColor(ImGuiCol_FrameBg, { 0, 0, 0, 0 });
				if (hovered[id] == i && any_hovered)
					PushStyleColor(ImGuiCol_FrameBg, { 0, 0, 0, 0 });

				AlignTextToFramePadding();
				Text(ICON_FK_BARS);

				SameLine(24.0f);
				SetNextItemWidth(GetContentRegionAvail().x);
				std::string input_hash = std::string("##input_text") + std::to_string(i);

				PushStyleColor(ImGuiCol_TextSelectedBg, GetStyleColorVec4(ImGuiCol_BorderShadow));

				InputText(input_hash.c_str(), &list[i]);

				PopStyleColor();

				if (is_selected)
					PopStyleColor();
				if (hovered[id] == i && any_hovered)
					PopStyleColor();

				PopID();
			}
		}

		if (open)
		{
			if (active[id] != -1 && hovered[id] != -1 && hovered[id] != active[id])
			{
				std::string item = list[active[id]];
				int n_next = active[id] + (GetMouseDragDelta(0).y < 0.1f ? -1 : 1);
				if (n_next >= 0 && n_next < list.size())
				{
					list[active[id]] = list[n_next];
					list[n_next] = item;
					ResetMouseDragDelta();
					active[id] = n_next;
					clicked[id] = n_next;
				}
			}

			if (IsMouseReleased(ImGuiMouseButton_Left))
			{
				active[id] = -1;
			}
		}

		EndCollapsibleGroupPanel();

		if (open)
		{
			PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
			PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
			ImVec4 c = GetStyleColorVec4(ImGuiCol_Border);
			PushStyleColor(ImGuiCol_Button, { c.x, c.y, c.z, c.w / 2.5f });

			Dummy({ 1.0f, 0.0f });
			SameLine(GetContentRegionAvail().x - 50.0f);

			if (Button(ICON_FK_PLUS))
			{
				list.push_back("");
			}
			SameLine();
			if (Button(ICON_FK_MINUS))
			{
				if (clicked[id] != -1)
					Chroma::RemoveAtIndex<std::vector<std::string>>(list, clicked[id]);
				if (clicked[id] > static_cast<int>(list.size()) - 1)
					clicked[id] = static_cast<int>(list.size()) - 1;
			}

			PopStyleColor();
			PopStyleVar(2);
		}

		PopID();

		return open;
	}
}
