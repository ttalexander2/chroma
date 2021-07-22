#include "Inspector.h"
#include <Chroma.h>
#include "Hierarchy.h"
#include <imgui.h>
#include <string>
#include "Fonts/IconsForkAwesome.h"
#include <Chroma/ImGui/ImGuiDebugMenu.h>
#include <Chroma/ImGui/ImGuiHelper.h>
#include <Chroma/Scene/Inspectable.h>

namespace Polychrome
{
	bool Inspector::Open = true;

	void Inspector::Draw()
	{
		if (Open)
		{
			ImGui::Begin("Inspector", &Inspector::Open);

			if (Hierarchy::SelectedEntity != Chroma::ENTITY_NULL)
			{
				DrawEntity();
			}

			ImGui::End();
		}

	}

	void Inspector::DrawEntity()
	{
		int unique = 69;
		Chroma::Scene* scene = Chroma::Application::Get().m_ActiveScene;


		if (Hierarchy::SelectedEntity == Chroma::ENTITY_NULL)
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 2, 2 });

		ImGui::BeginTable("##Inspector_table", 2, ImGuiTableFlags_None | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoClip);
		ImGui::TableNextColumn();

		for (Chroma::ComponentRef<Chroma::Component> c : scene->GetAllComponents(Hierarchy::SelectedEntity))
		{
			if (c->IsTag())
				continue;


			ImGui::TableSetColumnIndex(0);


			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0.01, 0.01 });
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 8 });

			ImGui::AlignTextToFramePadding();

			auto icon = ICON_FK_CARET_RIGHT;
			if (c->editor_inspector_open)
				icon = ICON_FK_CARET_DOWN;

			bool selected = true;
			if (ImGui::Selectable((std::string(" ") + icon + std::string("  ") + c->Name() + "##" + std::to_string(unique)).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
			{
				c->editor_inspector_open = !c->editor_inspector_open;
			}

			ImGui::PopStyleVar(2);

			ImGui::AlignTextToFramePadding();

			ImGui::SameLine(ImGui::GetWindowWidth() - 50);
			
			//auto color = ImGui::GetStyleColorVec4(ImGuiCol_Button);
			//ImGui::PushStyleColor(ImGuiCol_Button, { color.x, color.y, color.z, 0.99f });

			std::string iconVisible;
			if (c->editor_visible)
				iconVisible = ICON_FK_EYE;
			else
				iconVisible = ICON_FK_EYE_SLASH;

			if (ImGui::Button((iconVisible + "##MENU_BAR_INSPECTOR_BUTTON_VISIBLE" + std::to_string(unique)).c_str()))
			{
				c->editor_visible = !c->editor_visible;
			}

			ImGui::SameLine(ImGui::GetWindowWidth() - 26);


			if (ImGui::Button((ICON_FK_COG "##MENU_BAR_INSPECTOR_BUTTON_" + std::to_string(unique)).c_str()))
			{
				ImGui::OpenPopup(("##MENU_BAR_INSPECTOR_" + std::to_string(unique)).c_str());
			}

			//ImGui::PopStyleColor();

			if (ImGui::BeginPopup(("##MENU_BAR_INSPECTOR_" + std::to_string(unique)).c_str()))
			{
				if (!c->IsTransform())
				{
					if (ImGui::MenuItem(("Delete##MENU_BAR_INSPECTOR_" + std::to_string(unique)).c_str()))
					{
						c.Delete();
					}
				}

				ImGui::EndPopup();
			}

			




			if (c->editor_inspector_open)
			{
				ImGui::TableNextRow();
				ImGui::PushID(unique);
				c->DrawImGui();
				ImGui::PopID();
			}

			ImGui::TableNextRow();

			unique++;
		}

		ImGui::EndTable();

		ImGui::PopStyleVar();






		if (Hierarchy::SelectedEntity != Chroma::ENTITY_NULL && ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("##ENTITY_ADD_COMPONENT");
		}

		if (ImGui::BeginPopup("##ENTITY_ADD_COMPONENT"))
		{
			auto names = scene->GetComponentNames();
			auto ids = scene->GetComponentNamestoIDMap();

			for (auto& [name, func] : scene->GetComponentFactory())
			{
				bool enabled = true;
				if (scene->HasComponent(Hierarchy::SelectedEntity, ids[name]) && !scene->ComponentAllowsMultiple(ids[name]))
				{
					enabled = false;
				}

				if (ImGui::MenuItem((name + "##ENTITY_ADD_COMPONENT").c_str(), "", false, enabled))
				{
					func(*scene, Hierarchy::SelectedEntity);
				}
			}
			ImGui::EndPopup();
		}
	}
}