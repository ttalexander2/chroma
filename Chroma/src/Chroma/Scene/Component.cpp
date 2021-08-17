#include "chromapch.h"
#include "Component.h"
#include "imgui.h"
#include "Chroma/ImGui/IconsForkAwesome.h"
#include <Chroma/ImGui/ImGuiHelper.h>
#include "Chroma/Scene/ECS.h"

namespace Chroma
{
	unsigned int Component::component_counter = 1;

	bool Component::show_context = false;
	std::function<void(const std::string&, unsigned int)> Component::context_function;


	/// @brief Function to begin the serialization process for the component.
	/// 
	/// This function begins the key/map structure for the serialization of data members.
	/// @param out YAML emitter to write to
	void Component::BeginSerialize(YAML::Emitter& out)
	{
		out << YAML::Key << Name();
		out << YAML::Value << YAML::BeginMap;
	}

	/// @brief Function to end the serialization process for the component.
	/// 
	/// This function ends the key/map structure for the serialization of data members.
	/// @param out YAML emitter to write to
	void Component::EndSerialize(YAML::Emitter& out)
	{
		out << YAML::EndMap;
	}

	void Component::DoSerialize(YAML::Emitter& out)
	{
		BeginSerialize(out);
		Serialize(out);
		EndSerialize(out);
	}

	/// @brief Draws a component lable for the Editor component widgets.
	/// 
	/// This will draw the label in the left column of the componet widgets, and the next ImGui component will be drawn on the right.
	/// Usage:
	/// ```{.cpp}
	/// DrawComponentValue("Name");
	/// ImGui::Text("##name", Name);
	/// ```
	/// Note: ## is added in front of the ImGui::Text label so that the label is not drawn.
	/// @param label Label for the component.
	void Component::DrawComponentValue(std::string label)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		if (show_context)
		{
			context_function(label, comparison_id);
		}

		ImGui::TableSetColumnIndex(1);
	}


	//Used in DrawComponentValueCollapsible() to check if a collapsable label is open.
	static std::unordered_map <ImGuiID, bool> collapsibleOpen;

	/// @brief Draws a collapsable component lable for the Editor component widgets.
	/// 
	/// This will draw the label in the left column of the componet widgets, and the next ImGui component will be drawn on the right.
	/// Usage:
	/// ```{.cpp}
	/// DrawComponentValueCollapsible("Name");
	/// ImGui::Text("##name", Name);
	/// ```
	/// Note: ## is added in front of the ImGui::Text label so that the label is not drawn.
	/// @param label Label for the component.
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

		if (show_context)
		{
			context_function(label, comparison_id);
		}

		ImGui::TableSetColumnIndex(1);
		ImGui::NewLine();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		ImGui::PopID();

		return collapsibleOpen[id];

	}
}