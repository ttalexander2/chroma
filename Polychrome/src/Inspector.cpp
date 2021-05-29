#include "Inspector.h"
#include <Chroma.h>
#include "Hierarchy.h"
#include <imgui.h>
#include <string>
#include "Fonts/IconsForkAwesome.h"

namespace Polychrome
{
	bool Inspector::Open = true;

	void Inspector::Draw()
	{
		ImGui::Begin("Inspector", &Inspector::Open);
		int unique = 0;
		Chroma::EntityRef selected = Hierarchy::SelectedEntity;
		if (Hierarchy::SelectedEntity.GetID() == Chroma::ENTITY_NULL)
		{
			ImGui::End();
			return;
		}
			
		for (Chroma::ComponentRef<Chroma::Component> c : selected.GetAllComponents())
		{
			if (c->IsTag())
				continue;

			bool open = ImGui::CollapsingHeader((c->Name() + "##" + std::to_string(unique)).c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine(ImGui::GetWindowWidth() - 24);


			if (ImGui::Button((ICON_FK_COG "##MENU_BAR_INSPECTOR_BUTTON_" + std::to_string(unique)).c_str()))
			{
				ImGui::OpenPopup(("##MENU_BAR_INSPECTOR_" + std::to_string(unique)).c_str());
			}

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

			if (open)
			{
				c->DrawImGui();
			}



			unique++;
		}

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("##ENTITY_ADD_COMPONENT");
		}

		if (ImGui::BeginPopup("##ENTITY_ADD_COMPONENT"))
		{
			auto scene = Chroma::Application::Get().m_ActiveScene;
			auto names = scene->GetComponentNames();
			auto ids = scene->GetComponentNamestoIDMap();

			for (auto& [name, func] : scene->GetComponentFactory())
			{
				bool enabled = true;
				if (selected.HasComponent(ids[name]) && !scene->ComponentAllowsMultiple(ids[name]))
				{
					enabled = false;
				}

				if (ImGui::MenuItem((name + "##ENTITY_ADD_COMPONENT").c_str(), "", false, enabled))
				{						
					func(*scene, selected);
				}
			}
			ImGui::EndPopup();
		}

		ImGui::End();
	}
}