#include "Hierarchy.h"

#include "imgui.h"

#include "EditorApp.h"
#include <Chroma/Utilities/StringHelper.h>
#include <Chroma/Components/Tag.h>
#include <Chroma/Scene/Component.h>
#include <Chroma/Scene/ECS.h>


namespace Polychrome
{
	bool Hierarchy::Open = true;
	Chroma::EntityRef Hierarchy::SelectedEntity = Chroma::EntityRef(Chroma::ENTITY_NULL);

	void Hierarchy::Draw()
	{
		ImGui::Begin("Scene", &Hierarchy::Open);

		Chroma::Scene* scene = Chroma::Application::Get().m_ActiveScene;

		auto view = scene->GetEntities();

		static Chroma::EntityID renaming = Chroma::ENTITY_NULL;

		std::vector<Chroma::EntityID> toRemove;

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
						buffer = new char[512];
						strcpy(buffer, t->EntityName.c_str());
						ImGui::SetKeyboardFocusHere();
					}

					ImGui::MenuItem("Duplicate##HIERARCHY_CONTEXT_MENU");
					ImGui::MenuItem("Create Prefab##HIERARCHY_CONTEXT_MENU");
					ImGui::Separator();
					if (ImGui::MenuItem("Delete##HIERARCHY_CONTEXT_MENU"))
					{
						toRemove.push_back(e);
					}

					ImGui::EndPopup();
				}
			}

				


		}

		for (Chroma::EntityID id : toRemove)
		{
			scene->DestroyEntity(id);
			if (Hierarchy::SelectedEntity.GetID() == id)
				Hierarchy::SelectedEntity = Chroma::EntityRef(Chroma::ENTITY_NULL);
		}
		

		ImGui::End();
	}
}


