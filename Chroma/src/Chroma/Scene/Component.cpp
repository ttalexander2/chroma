#include "chromapch.h"
#include "Component.h"
#include "imgui.h"
#include "Chroma/ImGui/IconsForkAwesome.h"
#include <Chroma/ImGui/ImGuiHelper.h>

namespace Chroma
{
	unsigned int Component::component_counter = 1;

	void Component::BeginSerialize(YAML::Emitter& out)
	{
		out << YAML::Key << Name();
		out << YAML::Value << YAML::BeginMap;
	}
	void Component::EndSerialize(YAML::Emitter& out)
	{
		out << YAML::EndMap;
	}
	void Component::DrawComponentValue(std::string label)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::TableSetColumnIndex(1);
	}

	static std::unordered_map <ImGuiID, bool> collapsibleOpen;

	bool Component::DrawComponentValueCollapsible(std::string label)
	{
		ImGui::PushID(label.c_str());
		ImGuiID id = ImGui::GetID(label.c_str());
		if (collapsibleOpen.size() > 100)
			collapsibleOpen.clear();
		if (!collapsibleOpen.contains(id))
			collapsibleOpen.emplace(id, true);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 7 });
		ImGui::PushStyleColor(ImGuiCol_Header, { 0,0,0,0 });
		bool selected = true;
		auto icon = ICON_FK_CARET_RIGHT;
		if (collapsibleOpen[id])
			icon = ICON_FK_CARET_DOWN;
		if (ImGui::Selectable((std::string(" ") + icon + std::string("  ") + std::string(label)).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
		{
			collapsibleOpen[id] = !collapsibleOpen[id];
		}
		ImGui::TableSetColumnIndex(1);
		ImGui::NewLine();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		ImGui::PopID();

		return collapsibleOpen[id];

	}
}