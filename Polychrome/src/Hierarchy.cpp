#include "Hierarchy.h"

#include "imgui.h"

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
	/*
	void DrawTreeLevel(Chroma::Tree<Chroma::EntityID> tree, Chroma::Scene* scene, std::vector<Chroma::EntityID> &toRemove, Chroma::EntityID &renaming)
	{
		
		Chroma::ComponentRef<Chroma::Tag> t = scene->GetComponent<Chroma::Tag>(tree.Value);

		static bool valid = true;
		static std::string buffer;

		if (renaming == tree.Value)
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

			bool input = ImGui::InputText(("##HIERARCHY_RENAMING_ITEM_" + std::to_string(tree.Value)).c_str(), &buffer, ImGuiInputTextFlags_EnterReturnsTrue);

			//trim whitespace
			std::string trim(buffer);
			trim = std::TrimLeftWhitespace(trim);
			trim = std::TrimRightWhitespace(trim);

			//validate entity name
			bool loopValid = true;
			for (Chroma::EntityID entityToValidate : scene->View<Chroma::Tag>())
			{
				Chroma::ComponentRef<Chroma::Tag> tagToValidate = scene->GetComponent<Chroma::Tag>(entityToValidate);
				if (tree.Value != entityToValidate && tagToValidate->EntityName == std::string(buffer))
				{
					loopValid = false;
					break;
				}
			}

			ImGui::PopStyleColor();

			//check for submission
			if (valid && (input || !ImGui::IsItemActive()))
			{
				t->EntityName = buffer;
				renaming = Chroma::ENTITY_NULL;
			}

			valid = loopValid;

		}
		else
		{
			bool is_selected = Hierarchy::SelectedEntity.GetID() == tree.Value;
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick;
			if (is_selected)
				flags |= ImGuiTreeNodeFlags_Selected;
			if (tree.Children.size() == 0)
				flags |= ImGuiTreeNodeFlags_Leaf;
			if (ImGui::TreeNodeEx(t->EntityName.c_str(), flags))
			{
				for (Chroma::Tree<Chroma::EntityID> child : tree.Children)
				{
					DrawTreeLevel(child, scene, toRemove, renaming);
				}
				ImGui::TreePop();
			}
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup(("##HIERARCHY_CONTEXT_POPUP_" + std::to_string(tree.Value)).c_str());
			}
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			{
				Hierarchy::SelectedEntity = Chroma::Entity(tree.Value, *scene);
				Inspector::SetInspectable(Hierarchy::SelectedEntity);
			}

			if (ImGui::BeginPopupContextItem(("##HIERARCHY_CONTEXT_POPUP_" + std::to_string(tree.Value)).c_str()))
			{
				if (ImGui::MenuItem("New Child##HIRERARCHY_CONTEXT_MENU"))
				{
					auto cc = tree.Children;
					scene->NewEntityAsChild(tree.Value);
				}
				if (ImGui::MenuItem("Rename##HIERARCHY_CONTEXT_MENU"))
				{
					renaming = tree.Value;
					valid = false;
					buffer = t->EntityName;
					ImGui::SetKeyboardFocusHere();
				}

				ImGui::MenuItem("Duplicate##HIERARCHY_CONTEXT_MENU");
				ImGui::MenuItem("Create Prefab##HIERARCHY_CONTEXT_MENU");
				ImGui::Separator();
				if (ImGui::MenuItem("Delete##HIERARCHY_CONTEXT_MENU"))
				{
					toRemove.push_back(tree.Value);
				}

				ImGui::EndPopup();
			}
		}
	}
	*/
	void Hierarchy::Draw()
	{

		if (Hierarchy::Open)
		{
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
		}

		
	}
}


