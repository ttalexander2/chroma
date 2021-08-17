#include "Hierarchy.h"

#include "imgui.h"
#include <imgui_internal.h>

#include "EditorApp.h"
#include <Chroma/Utilities/StringHelper.h>
#include <Chroma/Components/Tag.h>
#include <Chroma/Scene/Component.h>
#include <Chroma/Scene/ECS.h>
#include "Inspector.h"
#include "imgui_stdlib.h"


namespace Polychrome
{
	bool Hierarchy::Open = true;
	Chroma::EntityID Hierarchy::SelectedEntity = Chroma::ENTITY_NULL;
	
	void Hierarchy::Draw()
	{
	


		if (Hierarchy::Open)
		{
			if (EditorApp::SceneRunning)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
			}
			ImGui::Begin("Scene", &Hierarchy::Open);
		
			Chroma::Scene* scene = EditorApp::CurrentScene;

			auto view = scene->Registry.view<Chroma::Tag>();


			static Chroma::EntityID renaming = Chroma::ENTITY_NULL;

			std::vector<Chroma::EntityID> toRemove;

			/*
			for (Chroma::Tree<Chroma::EntityID> tree : view.Children)
			{
				DrawTreeLevel(tree, scene, toRemove, renaming);
			}
			*/





			for (Chroma::EntityID e : view)
			{
				Chroma::Tag& t = scene->GetComponent<Chroma::Tag>(e);

				static bool valid = true;
				static std::string buffer;

				if (renaming == e)
				{
					if (!valid)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });
					}


					if (!valid)
						ImGui::SetKeyboardFocusHere();

					bool input = ImGui::InputText(("##HIERARCHY_RENAMING_ITEM_" + std::to_string((uint32_t)e)).c_str(), &buffer, ImGuiInputTextFlags_EnterReturnsTrue);

					//trim whitespace
					buffer = std::TrimLeftWhitespace(buffer);
					buffer = std::TrimRightWhitespace(buffer);

					//validate entity name
					bool loopValid = true;
					for (Chroma::EntityID entityToValidate : scene->Registry.view<Chroma::Tag>())
					{
						Chroma::Tag& tagToValidate = scene->GetComponent<Chroma::Tag>(entityToValidate);
						if (e != entityToValidate && tagToValidate.EntityName == std::string(buffer))
						{
							loopValid = false;
							break;
						}
					}

					ImGui::PopStyleColor();

					//check for submission
					if (valid && (input || !ImGui::IsItemActive()))
					{
						t.EntityName = buffer;
						renaming = Chroma::ENTITY_NULL;
					}

					valid = loopValid;

				}
				else
				{
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Leaf;
					if (SelectedEntity == e)
						flags |= ImGuiTreeNodeFlags_Selected;

					bool opened = ImGui::TreeNodeEx(t.EntityName.c_str(), flags);

					if (ImGui::IsItemClicked())
					{
						Hierarchy::SelectedEntity = e;
						Chroma::Entity r = Chroma::Entity(e, scene);
					}

					if (opened)
						ImGui::TreePop();

					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
					{
						ImGui::OpenPopup(("##HIERARCHY_CONTEXT_POPUP_" + std::to_string((uint32_t)e)).c_str());
					}

					if (ImGui::BeginPopupContextItem(("##HIERARCHY_CONTEXT_POPUP_" + std::to_string((uint32_t)e)).c_str()))
					{
						if (ImGui::MenuItem("Rename##HIERARCHY_CONTEXT_MENU"))
						{
							renaming = e;
							valid = false;
							buffer = t.EntityName;
							ImGui::SetKeyboardFocusHere();
						}

						ImGui::MenuItem("Duplicate##HIERARCHY_CONTEXT_MENU");
						ImGui::MenuItem("Create Prefab##HIERARCHY_CONTEXT_MENU");
						ImGui::Separator();
						if (ImGui::MenuItem("Delete##HIERARCHY_CONTEXT_MENU"))
						{
							scene->DestroyEntity(e);
							Hierarchy::SelectedEntity = Chroma::ENTITY_NULL;
						}

						ImGui::EndPopup();
					}
				}

			}


			ImGui::End();

			if (EditorApp::SceneRunning)
			{
				ImGui::PopStyleColor();
			}
		}

		
	}
}


