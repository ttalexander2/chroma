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
#include <Chroma/Components/Relationship.h>
#include <Chroma/Utilities/ContainerHelpers.h>


namespace Polychrome
{
	bool Hierarchy::Open = true;
	Chroma::EntityID Hierarchy::SelectedEntity = Chroma::ENTITY_NULL;
	static Chroma::EntityID renaming = Chroma::ENTITY_NULL;
	static Chroma::EntityID hovered = Chroma::ENTITY_NULL;
	static bool any_hovered = false;
	static bool has_entity_payload = false;
	static bool any_separator_hovered = false;
	static bool separator_child = false;
	static Chroma::EntityID separator_curr = Chroma::ENTITY_NULL;
	static Chroma::EntityID separator_next = Chroma::ENTITY_NULL;

	static std::vector<Chroma::EntityID> to_remove_from_order;

	void Hierarchy::Draw()
	{



		if (Hierarchy::Open)
		{
			if (EditorApp::SceneRunning)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			}
			ImGui::Begin("Scene", &Hierarchy::Open);

			Chroma::Scene* scene = EditorApp::CurrentScene;

			auto view = scene->Registry.view<Chroma::Tag>();

			std::vector<Chroma::EntityID> toRemove;



			any_hovered = false;
			any_separator_hovered = false;
			separator_child = false;

			has_entity_payload = ImGui::GetDragDropPayload() != nullptr && ImGui::GetDragDropPayload()->IsDataType("Entity");

			if (scene->EntityOrder.size() > 0)
				DragDropSeparator(scene, Chroma::ENTITY_NULL, scene->EntityOrder[0], 3.f);

			std::vector<Chroma::EntityID> entities(scene->EntityOrder);

			int i = 0;
			for (Chroma::EntityID e : entities)
			{
				if (entities.size() > i + 1)
					DrawEntity(scene, e, entities[i + 1]);
				else
					DrawEntity(scene, e, Chroma::ENTITY_NULL);
				i++;
			}

			if (!any_hovered)
				hovered = Chroma::ENTITY_NULL;

			if (!any_separator_hovered)
			{
				separator_curr = Chroma::ENTITY_NULL;
				separator_next = Chroma::ENTITY_NULL;
			}

			for (Chroma::EntityID ent : to_remove_from_order)
			{
				Chroma::PopValue(scene->EntityOrder, ent);	
			}
			to_remove_from_order.clear();

			ImGui::End();

			if (EditorApp::SceneRunning)
			{
				ImGui::PopStyleColor();
				ImGui::PopItemFlag();
			}
		}


	}




	void Hierarchy::DrawEntity(Chroma::Scene* scene, Chroma::EntityID e, Chroma::EntityID next, bool root)
	{
		if (e == Chroma::ENTITY_NULL)
			return;
		auto& relationship = scene->GetComponent<Chroma::Relationship>(e);
		Chroma::Tag& t = scene->GetComponent<Chroma::Tag>(e);

		if (root && relationship.IsChild())
			return;

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


			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
			if (!relationship.HasChildren())
				flags |= ImGuiTreeNodeFlags_Leaf;
			if (SelectedEntity == e)
				flags |= ImGuiTreeNodeFlags_Selected;

			if (!has_entity_payload)
				flags |= ImGuiTreeNodeFlags_FramePadding;

			const ImGuiPayload* drop = ImGui::GetDragDropPayload();
			bool valid_root = true;
			if ((hovered == e) && drop != nullptr && drop->IsDataType("Entity"))
			{
				valid_root = IsRelative(scene, (*((Chroma::EntityID*)drop->Data)), e);
			}


			bool hover_push = (hovered == e) && drop != nullptr && drop->IsDataType("Entity") && (*((Chroma::EntityID*)drop->Data)) != e && !valid_root;
			if (hover_push)
			{
				if (relationship.HasChildren())
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0.2f, 0.8f, 0.3f, .7f });
				else
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.2f, 0.8f, 0.3f, .7f });
				flags |= ImGuiTreeNodeFlags_Selected;
			}
			else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				if (relationship.HasChildren())
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0,0,0,0 });
				else
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0,0,0,0 });
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 4 });

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());

			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);
			bool opened = ImGui::TreeNodeEx(t.EntityName.c_str(), flags);
			ImGui::PopItemFlag();

			ImGui::PopStyleVar();

			if (hover_push || ImGui::IsMouseDragging(ImGuiMouseButton_Left))
				ImGui::PopStyleColor();

			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
			{
				hovered = e;
				any_hovered = true;
			}

			ImGui::PushID(t.EntityName.c_str());
			if (ImGui::BeginPopupContextItem())
			{
				ImGui::Text("Move entity...");
				ImGui::EndPopup();
			}
			ImGui::PopID();

			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);
			if (ImGui::IsItemClicked() || ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				Hierarchy::SelectedEntity = e;
			}
			ImGui::PopItemFlag();


			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup(("##HIERARCHY_CONTEXT_POPUP_" + std::to_string((uint32_t)e)).c_str());
			}

			if (hover_push && ImGui::BeginDragDropTarget() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				const ImGuiPayload* payload = ImGui::GetDragDropPayload();
				if (payload->IsDataType("Entity"))
				{
					Chroma::EntityID data = (*((Chroma::EntityID*)payload->Data));
					//CHROMA_CORE_TRACE("DROPPED AT {}. ACCEPTED {}", e, data);

					//Get payload's parent
					auto p = scene->GetComponent<Chroma::Relationship>(data).Parent;

					if (p == Chroma::ENTITY_NULL)
					{
						to_remove_from_order.push_back(data);
					}
					else
					{
						//Remove payload from parent's child list
						Chroma::PopValue(scene->GetComponent<Chroma::Relationship>(p).Children, data);
					}
						
					//Add to target's children
					relationship.Children.push_back(data);
					//Set payload's parent
					scene->GetComponent<Chroma::Relationship>(data).Parent = e;
					
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("Entity", (void*)&e, sizeof(Chroma::EntityID));
				ImGui::EndDragDropSource();
			}

			bool open_modal = false;

			if (ImGui::BeginPopupContextItem(("##HIERARCHY_CONTEXT_POPUP_" + std::to_string((uint32_t)e)).c_str()))
			{
				if (ImGui::MenuItem("Rename##HIERARCHY_CONTEXT_MENU"))
				{
					renaming = e;
					valid = false;
					buffer = t.EntityName;
					ImGui::SetKeyboardFocusHere();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("New Entity##HIERARCHY_CONTEXT_MENU"))
					scene->NewEntity();
				if (ImGui::MenuItem("New Child##HIERARCHY_CONTEXT_MENU"))
					scene->NewChild(e);
				if (ImGui::BeginMenu("New Entity...##HIERARCHY_CONTEXT_MENU"))
				{
					for (auto& component : Chroma::ECS::GetComponentNames())
					{
						if (component == "Transform" || component == "Relationship" || component == "Tag")
							continue;

						if (ImGui::MenuItem(component.c_str()))
						{
							Chroma::Entity ent = scene->NewEntity();
							ent.AddComponent(component);
						}
					}
					ImGui::EndMenu();
				}
					
				if (ImGui::BeginMenu("New Child...##HIERARCHY_CONTEXT_MENU"))
				{
					for (auto& component : Chroma::ECS::GetComponentNames())
					{
						if (component == "Transform" || component == "Relationship" || component == "Tag")
							continue;

						if (ImGui::MenuItem(component.c_str()))
						{
							Chroma::Entity ent = scene->NewChild(e);
							ent.AddComponent(component);
						}
					}
					ImGui::EndMenu();
					
				}
					
				ImGui::Separator();

				ImGui::MenuItem("Duplicate##HIERARCHY_CONTEXT_MENU");
				ImGui::MenuItem("Create Prefab##HIERARCHY_CONTEXT_MENU");
				ImGui::Separator();
				if (ImGui::MenuItem("Delete##HIERARCHY_CONTEXT_MENU"))
				{
					open_modal = true;
				}

				ImGui::EndPopup();
			}

			if (open_modal)
				ImGui::OpenPopup("Delete Entitiy?");

			if (ImGui::BeginPopupModal("Delete Entitiy?", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Are you sure you want to delete this entity? It cannot be undone.");



				if (ImGui::Button("Delete"))
				{
					scene->DestroyEntity(e);
					Hierarchy::SelectedEntity = Chroma::ENTITY_NULL;
				}
				if (relationship.HasChildren())
				{
					ImGui::SameLine();
					if (ImGui::Button("Delete (include children)"))
					{
						scene->DestroyEntity(e, true);
						Hierarchy::SelectedEntity = Chroma::ENTITY_NULL;
					}
				}

					
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			if (opened)
			{
				if (relationship.HasChildren())
				{
					DragDropSeparator(scene, e, relationship.Children[0], 7.0f);

					std::vector<Chroma::EntityID> children_copy(relationship.Children);
					int i = 0;
					for (Chroma::EntityID child : children_copy)
					{
						if (children_copy.size() > i + 1)
							DrawEntity(scene, child, children_copy[i + 1], false);
						else
							DrawEntity(scene, child, Chroma::ENTITY_NULL, false);
						i++;
					}
				}
				ImGui::TreePop();
			}

			if (relationship.Parent == Chroma::ENTITY_NULL || next != Chroma::ENTITY_NULL)
				DragDropSeparator(scene, e, next);

		}
	}

	bool Hierarchy::IsRelative(Chroma::Scene* scene, Chroma::EntityID parent, Chroma::EntityID child)
	{
		if (parent == Chroma::ENTITY_NULL || child == Chroma::ENTITY_NULL || scene == nullptr)
			return false;

		auto& p_rel = scene->GetComponent<Chroma::Relationship>(parent);
		auto& c_rel = scene->GetComponent<Chroma::Relationship>(child);

		auto& cp = child;

		while (cp != Chroma::ENTITY_NULL)
		{
			auto rpp = scene->GetComponent<Chroma::Relationship>(cp).Parent;
			if (rpp == parent)
				return true;
			cp = rpp;
		}
		return false;
	}

	void Hierarchy::DragDropSeparator(Chroma::Scene* scene, Chroma::EntityID curr, Chroma::EntityID next, float size)
	{
		if (!has_entity_payload)
			return;

		const ImGuiPayload* paydrop = ImGui::GetDragDropPayload();
		if (paydrop != nullptr && paydrop->IsDataType("Entity"))
		{
			//CHROMA_CORE_INFO("Current: {}, Next: {}", curr, next);
			auto id = (*((Chroma::EntityID*)paydrop->Data));
			if (next != Chroma::ENTITY_NULL && scene->GetComponent<Chroma::Relationship>(next).Parent == id)
			{
				ImGui::Dummy({ ImGui::GetContentRegionAvailWidth(), size });
				return;
			}
			else if (id == curr || id == next)
			{
				ImGui::Dummy({ ImGui::GetContentRegionAvailWidth(), size });
				return;
			}

		}

		ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0,0,0,0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0,0,0,0 });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);

		const ImGuiPayload* payload = ImGui::GetDragDropPayload();

		bool is_parent = false;

		if (paydrop != nullptr && paydrop->IsDataType("Entity"))
		{
			Chroma::EntityID drop_id = (*((Chroma::EntityID*)payload->Data));
			is_parent = IsRelative(scene, drop_id, curr);
			
		}
	



		bool hover_push = (separator_curr == curr) && (separator_next == next) && paydrop != nullptr && paydrop->IsDataType("Entity") && (*((Chroma::EntityID*)paydrop->Data)) != curr && (*((Chroma::EntityID*)paydrop->Data)) != next && !is_parent;
		if (hover_push)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.8f, 0.3f, .7f });
		}

		ImGui::Button("##separator_button", { ImGui::GetContentRegionAvailWidth(), size });
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			separator_curr = curr;
			separator_next = next;
			any_separator_hovered = true;
		}

		if (ImGui::BeginDragDropTarget() && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && hover_push)
		{
			Chroma::EntityID drop_id = (*((Chroma::EntityID*)payload->Data));
			auto& drop_rel = scene->GetComponent<Chroma::Relationship>(drop_id);
			auto& drop_parent = drop_rel.Parent;


			if (payload->IsDataType("Entity") && !is_parent)
			{
				
				if (drop_parent == Chroma::ENTITY_NULL)
				{
					Chroma::PopValue(scene->EntityOrder, drop_id);
				}
				else
				{
					auto& dp = scene->GetComponent<Chroma::Relationship>(drop_parent).Children;
					Chroma::PopValue(dp, drop_id);
					int x = 0;
				}

				//Dropped at top
				if (curr == Chroma::ENTITY_NULL && next != Chroma::ENTITY_NULL)
				{
					scene->EntityOrder.insert(scene->EntityOrder.begin(), drop_id);
					drop_rel.Parent = Chroma::ENTITY_NULL;
				}
				//Dropped at end
				else if (curr != Chroma::ENTITY_NULL && next == Chroma::ENTITY_NULL)
				{
					scene->EntityOrder.insert(scene->EntityOrder.end(), drop_id);
					drop_rel.Parent = Chroma::ENTITY_NULL;
				}
				else if (curr != Chroma::ENTITY_NULL && next != Chroma::ENTITY_NULL)
				{
					auto& curr_rel = scene->GetComponent<Chroma::Relationship>(curr);
					auto& next_rel = scene->GetComponent<Chroma::Relationship>(next);

					//curr is parent of next
					if (next_rel.Parent == curr)
					{
						curr_rel.Children.insert(curr_rel.Children.begin(), drop_id);
						drop_rel.Parent = curr;
					}
					else if (next_rel.Parent == curr_rel.Parent)
					{
						if (next_rel.Parent == Chroma::ENTITY_NULL)
						{
							scene->EntityOrder.insert(std::find(scene->EntityOrder.begin(), scene->EntityOrder.end(), next), drop_id);
							drop_rel.Parent = Chroma::ENTITY_NULL;
						}
						else
						{
							auto& children = scene->GetComponent<Chroma::Relationship>(next_rel.Parent).Children;
							children.insert(std::find(children.begin(), children.end(), next), drop_id);
							drop_rel.Parent = next_rel.Parent;
						}
					}
				}




				
			}
				
		}


		if (hover_push)
			ImGui::PopStyleColor();

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();
	}
}


