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
	Chroma::EntityRef Hierarchy::SelectedEntity = Chroma::EntityRef(Chroma::ENTITY_NULL);
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
		ImGui::Begin("Scene", &Hierarchy::Open);

		Chroma::Scene* scene = Chroma::Application::Get().m_ActiveScene;

		auto view = scene->GetEntities();

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
			Chroma::ComponentRef<Chroma::Tag> t = scene->GetComponent<Chroma::Tag>(e);

			static bool valid = true;
			static char* buffer = new char[512];

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

				bool input = ImGui::InputText(("##HIERARCHY_RENAMING_ITEM_" + std::to_string(e)).c_str(), buffer, 512, ImGuiInputTextFlags_EnterReturnsTrue);

				//trim whitespace
				std::string trim(buffer);
				trim = std::TrimLeftWhitespace(trim);
				trim = std::TrimRightWhitespace(trim);
				strcpy(buffer, trim.c_str());

				//validate entity name
				bool loopValid = true;
				for (Chroma::EntityID entityToValidate : scene->GetEntities())
				{
					Chroma::ComponentRef<Chroma::Tag> tagToValidate = scene->GetComponent<Chroma::Tag>(entityToValidate);
					if (e != entityToValidate && tagToValidate->EntityName == std::string(buffer))
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
					delete[] buffer;
				}

				valid = loopValid;

			}
			else
			{
				if (ImGui::Selectable(t->EntityName.c_str(), Hierarchy::SelectedEntity.GetID() == e))
				{
					Hierarchy::SelectedEntity = Chroma::EntityRef(e, *scene);
					Inspector::SetInspectable(Hierarchy::SelectedEntity);
				}
				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup(("##HIERARCHY_CONTEXT_POPUP_" + std::to_string(e)).c_str());
				}

				if (ImGui::BeginPopupContextItem(("##HIERARCHY_CONTEXT_POPUP_" + std::to_string(e)).c_str()))
				{
					if (ImGui::MenuItem("Rename##HIERARCHY_CONTEXT_MENU"))
					{
						renaming = e;
						valid = false;
						if (buffer != nullptr)
							delete[] buffer;
						buffer = new char[512];
						strcpy(buffer, t->EntityName.c_str());
						ImGui::SetKeyboardFocusHere();
					}

					ImGui::MenuItem("Duplicate##HIERARCHY_CONTEXT_MENU");
					ImGui::MenuItem("Create Prefab##HIERARCHY_CONTEXT_MENU");
					ImGui::Separator();
					if (ImGui::MenuItem("Delete##HIERARCHY_CONTEXT_MENU"))
					{
						scene->DestroyEntity(e);
					}

					ImGui::EndPopup();
				}
			}

		}
		

		ImGui::End();
	}
}


