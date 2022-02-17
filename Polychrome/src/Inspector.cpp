#include "Inspector.h"
#include <Chroma.h>
#include "Hierarchy.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <string>
#include "Fonts/IconsForkAwesome.h"
#include <Chroma/ImGui/ImGuiDebugMenu.h>
#include <Chroma/ImGui/ImGuiHelper.h>
#include "EditorApp.h"
#include "ComponentWidgets.h"
#include <Chroma/Components/Tag.h>
#include <Chroma/Components/CSharpScript.h>

namespace Polychrome
{
	bool Inspector::Open = true;

	void Inspector::Draw()
	{

		if (ComponentWidgets::context_function == nullptr)
		{
			std::function<void(const::std::string&, unsigned int)> func = [&](const std::string& component, unsigned int id) {
				ImGui::PushID(fmt::format("##id{}.{}", component, id).c_str());
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Text));

				if (ImGui::BeginPopupContextItem("##Component_popup_context", ImGuiPopupFlags_MouseButtonLeft))
				{
					ImGui::Text("This a popu!");
					ImGui::EndPopup();
				}

				ImGui::PopStyleColor();
				ImGui::PopID();
			};
			ComponentWidgets::context_function = func;
		}

		if (EditorApp::ScenePaused)
			ComponentWidgets::show_context = true;
		else
			ComponentWidgets::show_context = false;

		if (Open)
		{
			if (ImGui::Begin("Inspector", &Inspector::Open))
			{
				if (Hierarchy::SelectedEntity != Chroma::ENTITY_NULL)
				{
					if (EditorApp::SceneRunning && !EditorApp::ScenePaused)
					{
						//ImGui::PushItemFlag(ImGuiItemFlags_Disabled, EditorApp::SceneRunning && !EditorApp::ScenePaused);
					}
					if (EditorApp::ScenePaused || EditorApp::SceneRunning)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
					}

					ImFont* defaultFont = ImGui::GetIO().FontDefault;
					ImFont f = *EditorApp::LargeIcons;
					f.Scale = f.Scale * 0.2f;
					ImGui::SetCurrentFont(&f);
					
					ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive), ICON_FK_CUBE);
					ImGui::SetCurrentFont(defaultFont);
					
					
					ImGui::SameLine();

					Chroma::Tag& tagComp = EditorApp::CurrentScene->GetComponent<Chroma::Tag>(Hierarchy::SelectedEntity);
					ImGui::InputText("##entity_name", &tagComp.EntityName);

					//ImGui::SameLine();
					//
					//ImGui::BeginCombo("##entity_layer", "");


					DrawEntity();
					if (EditorApp::SceneRunning && !EditorApp::ScenePaused)
					{
						//ImGui::PopItemFlag();
					}
					if (EditorApp::ScenePaused || EditorApp::SceneRunning)
					{
						ImGui::PopStyleColor();
					}
				}
			}

			ImGui::End();
		}

	}

	void Inspector::DrawEntity()
	{
		int unique = 69;
		Chroma::Scene* scene = EditorApp::CurrentScene;


		if (Hierarchy::SelectedEntity == Chroma::ENTITY_NULL)
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 2, 2 });

		ImGui::BeginTable("##Inspector_table", 2, ImGuiTableFlags_None | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_NoClip);
		ImGui::TableNextColumn();

		std::vector<Chroma::Component*> comps(scene->GetAllComponents(Hierarchy::SelectedEntity));
		std::sort(comps.begin(), comps.end(), [](Chroma::Component* a, Chroma::Component* b) { return a->order_id < b->order_id; });

		int i = 0;

		for (Chroma::Component* c : comps)
		{
			if (c->IsTypeOf<Chroma::Relationship>() || c->IsTypeOf<Chroma::Tag>())
			{
				i++;
				continue;
			}
				


			ImGui::TableSetColumnIndex(0);


			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0.01, 0.01 });
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 8 });

			ImGui::AlignTextToFramePadding();

			auto icon = ICON_FK_CARET_RIGHT;
			if (c->editor_inspector_open)
				icon = ICON_FK_CARET_DOWN;

			bool selected = true;
			std::string comp_name(c->GetTypeName());
			if (comp_name == Chroma::CSharpScript::GetTypeNameStatic())
			{
				try
				{
					Chroma::CSharpScript* a = reinterpret_cast<Chroma::CSharpScript*>(c);
					comp_name = std::string(a->ModuleName);
				}
				catch (const std::exception& e)
				{
					CHROMA_CORE_WARN("{}", e.what());
				}
			}

			if (ImGui::Selectable((std::string(" ") + icon + std::string("  ") + comp_name + "##" + std::to_string(unique)).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
			{
				c->editor_inspector_open = !c->editor_inspector_open;
			}

			if (comp_name != Chroma::Transform::GetTypeNameStatic() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				ImGui::SetDragDropPayload("COMPONENT_REORDER", &i, sizeof(int));
				ImGui::EndDragDropSource();
			}
			if (comp_name != Chroma::Transform::GetTypeNameStatic() && ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("COMPONENT_REORDER"))
				{
					int payload_i = *(const int*)payload->Data;
					int payload_order = comps[payload_i]->order_id;
					int order = comps[i]->order_id;
					comps[payload_i]->order_id = order;
					comps[i]->order_id = payload_order;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::PopStyleVar(2);

			ImGui::AlignTextToFramePadding();

			//ImGui::SameLine(ImGui::GetWindowWidth() - 50);
			//
			////auto color = ImGui::GetStyleColorVec4(ImGuiCol_Button);
			////ImGui::PushStyleColor(ImGuiCol_Button, { color.x, color.y, color.z, 0.99f });
			//
			//std::string iconVisible;
			//if (c->editor_visible)
			//	iconVisible = ICON_FK_EYE;
			//else
			//	iconVisible = ICON_FK_EYE_SLASH;
			//
			//if (ImGui::Button((iconVisible + "##MENU_BAR_INSPECTOR_BUTTON_VISIBLE" + std::to_string(unique)).c_str()))
			//{
			//	c->editor_visible = !c->editor_visible;
			//}

			

			if (!c->IsTypeOf<Chroma::Transform>())
			{
				ImGui::SameLine(ImGui::GetWindowWidth() - 50);
				bool enabled = c->IsEnabled();
				ImGui::Checkbox(("##MENU_BAR_INSPECTOR_CHECKBOX_COMP" + std::to_string(unique)).c_str(), &enabled);
				if (c->IsEnabled() != enabled)
					c->SetEnabled(enabled);
			}



			ImGui::SameLine(ImGui::GetWindowWidth() - 30);


			if (ImGui::Button((ICON_FK_COG "##MENU_BAR_INSPECTOR_BUTTON_" + std::to_string(unique)).c_str()))
			{
				ImGui::OpenPopup(("##MENU_BAR_INSPECTOR_" + std::to_string(unique)).c_str());
			}

			//ImGui::PopStyleColor();

			if (ImGui::BeginPopup(("##MENU_BAR_INSPECTOR_" + std::to_string(unique)).c_str()))
			{
				if (!c->IsTypeOf<Chroma::Transform>())
				{
					if (ImGui::MenuItem(("Delete##MENU_BAR_INSPECTOR_" + std::to_string(unique)).c_str()))
					{
						scene->RemoveComponent(c->GetTypeName(), Hierarchy::SelectedEntity);
					}
				}

				ImGui::EndPopup();
			}

			




			if (c->editor_inspector_open)
			{
				ImGui::TableNextRow();
				ImGui::PushID(unique);

				ComponentWidgets::Draw(c);

				ImGui::PopID();
			}

			ImGui::TableNextRow();

			unique++;
			i++;
		}

		ImGui::EndTable();

		ImGui::PopStyleVar();






		if (Hierarchy::SelectedEntity != Chroma::ENTITY_NULL && ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("##ENTITY_ADD_COMPONENT");
		}

		if (ImGui::BeginPopup("##ENTITY_ADD_COMPONENT"))
		{
			for (auto name : Chroma::Scene::GetComponentTypes())
			{
				if (name->IsTypeOf<Chroma::Transform>() || name->IsTypeOf<Chroma::Tag>() || name->IsTypeOf<Chroma::Relationship>())
					continue;
				bool enabled = true;
				if (scene->HasComponent(name->GetTypeName(), Hierarchy::SelectedEntity))
					enabled = false;

				if (ImGui::MenuItem((name->GetTypeName() + "##ENTITY_ADD_COMPONENT").c_str(), "", false, enabled))
				{
					scene->AddComponent(name->GetTypeName(), Hierarchy::SelectedEntity);
				}
			}
			ImGui::EndPopup();
		}
	}
}