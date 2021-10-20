#include "ComponentWidgets.h"
#include <imgui.h>
#include <imgui_stdlib.h>
#include "Fonts/IconsForkAwesome.h"
#include <Chroma/Components/AudioSource.h>
#include <Chroma/Audio/Audio.h>
#include <Chroma/Components/Transform.h>
#include <Chroma/ImGui/Widgets/VecWithLabels.h>
#include <Chroma/Components/CSharpScript.h>
#include <Chroma/Scripting/ScriptModuleField.h>
#include <Chroma/Components/BoxCollider.h>
#include <Chroma/Components/CircleCollider.h>
#include <Chroma/Components/LuaScript.h>
#include <Chroma/Scripting/LuaScripting.h>
#include <Chroma/Scripting/MonoScripting.h>
#include <Chroma/Components/SpriteRenderer.h>
#include "EditorApp.h"
#include "Hierarchy.h"

namespace ImGui
{
	bool Combo(const char* label, int* current_item, const std::vector<std::string>& items, int items_count, int height_in_items = -1)
	{
		return Combo(label, current_item, [](void* data, int idx, const char** out_text) { *out_text = (*((const std::vector<std::string>*)data))[idx].c_str(); return true; }, (void*)&items, items_count, height_in_items);
	}
}

namespace Polychrome
{
	bool ComponentWidgets::show_context = false;
	std::function<void(const std::string&, unsigned int)> ComponentWidgets::context_function;
	static std::unordered_map <ImGuiID, bool> collapsibleOpen;

	void ComponentWidgets::Draw(Chroma::Component* c)
	{
		if (Chroma::ECS::IsType<Chroma::Transform>(c)) DrawTransform(c);
		else if (Chroma::ECS::IsType<Chroma::CSharpScript>(c)) DrawCSharpScript(c);
		else if (Chroma::ECS::IsType<Chroma::AudioSource>(c)) DrawAudioSource(c);
		else if (Chroma::ECS::IsType<Chroma::BoxCollider>(c)) DrawBoxCollider(c);
		else if (Chroma::ECS::IsType<Chroma::CircleCollider>(c)) DrawCircleCollider(c);
		else if (Chroma::ECS::IsType<Chroma::LuaScript>(c)) DrawLuaScript(c);
		else if (Chroma::ECS::IsType<Chroma::SpriteRenderer>(c)) DrawSpriteRenderer(c);
		else if (Chroma::ECS::IsType<Chroma::Camera>(c)) DrawCameraComponent(c);
	}

	void ComponentWidgets::DrawTransform(Chroma::Component* c)
	{
		Chroma::Transform* t = reinterpret_cast<Chroma::Transform*>(c);
		DrawComponentValue(t, "Position");
		ImGui::Vec2IntWithLabels(("##transform_position" + std::to_string(t->GetUniqueID())).c_str(), t->Position);

		DrawComponentValue(t, "Rotation");
		float rot = glm::degrees(t->Rotation);
		ImGui::SetNextItemWidth(ImGui::GetColumnWidth(2) - 2);
		ImGui::SliderFloat(("##transform_rotation" + std::to_string(t->GetUniqueID())).c_str(), &rot, 0.f, 360.f);
		t->Rotation = glm::radians(rot);

		DrawComponentValue(t, "Scale");
		ImGui::Vec2FloatWithLabels(("##transform_scale" + std::to_string(t->GetUniqueID())).c_str(), t->Scale, false);
	}

	void ComponentWidgets::DrawCSharpScript(Chroma::Component* c)
	{
		Chroma::CSharpScript* script = reinterpret_cast<Chroma::CSharpScript*>(c);
		if (script->ModuleName.empty())
			return;

		if (script->ModuleFieldMap.find(script->ModuleName) == script->ModuleFieldMap.end())
			return;

		if (EditorApp::SceneRunning)
		{
			Chroma::EntityID entity = Hierarchy::SelectedEntity;
			//Chroma::MonoScripting::getEntity
			auto& entityInstanceData = Chroma::MonoScripting::GetEntityInstanceData(EditorApp::CurrentScene->GetID(), entity);
			for (auto& [name, field] : script->ModuleFieldMap[script->ModuleName])
			{
				DrawComponentValue(script, name);
				const char* hash = ("##" + name).c_str();
				if (field.Type == Chroma::FieldType::Float)
				{
					float val = field.GetRuntimeValue<float>(entityInstanceData.Instance);
					ImGui::InputFloat(hash, &val);
					field.SetStoredValue<float>(val);
				}
				else if (field.Type == Chroma::FieldType::Int)
				{
					int val = field.GetRuntimeValue<int>(entityInstanceData.Instance);
					ImGui::InputInt(hash, &val);
					field.SetStoredValue<int>(val);
				}
				else if (field.Type == Chroma::FieldType::String)
				{
					std::string val = field.GetRuntimeValue<std::string>(entityInstanceData.Instance);
					ImGui::InputText(hash, &val);
					field.SetStoredValue(val);
				}
			}
		}
		else
		{
			for (auto& [name, field] : script->ModuleFieldMap[script->ModuleName])
			{
				DrawComponentValue(script, name);
				const char* hash = ("##" + name).c_str();
				if (field.Type == Chroma::FieldType::Float)
				{
					float val = field.GetStoredValue<float>();
					ImGui::InputFloat(hash, &val);
					field.SetStoredValue<float>(val);
				}
				else if (field.Type == Chroma::FieldType::Int)
				{
					int val = field.GetStoredValue<int>();
					ImGui::InputInt(hash, &val);
					field.SetStoredValue<int>(val);
				}
				else if (field.Type == Chroma::FieldType::String)
				{
					std::string val = field.GetStoredValue<const std::string&>();
					ImGui::InputText(hash, &val);
					field.SetStoredValue<const std::string&>(val);
				}
			}
		}
		
		//DrawComponentValue()
	}

	void ComponentWidgets::DrawAudioSource(Chroma::Component* c)
	{
		Chroma::AudioSource* a = reinterpret_cast<Chroma::AudioSource*>(c);
		ImGui::TableSetColumnIndex(0);

		ImGui::AlignTextToFramePadding(); ImGui::Text("Event");
		ImGui::AlignTextToFramePadding(); ImGui::Text("Mute");
		ImGui::AlignTextToFramePadding(); ImGui::Text("Play on Init");

		ImGui::TableSetColumnIndex(1);

		ImGui::PushItemWidth(0);

		auto hints = Chroma::Audio::GetEventPathList();
		if (hints.size() <= 0)
			return;

		std::sort(hints.begin(), hints.end());

		//static ComboFilterState s = { 0 };
		static char buf[512] = "type text here...";
		//if (ComboFilter("##AUDIO_SOURCE_COMPONENT_" + GetUniqueID(), buf, IM_ARRAYSIZE(buf), hints, hints.size(), s))
		//{
		//	Event = std::string(buf);
		//	GUID = Audio::GetEventGuid(Event);
		//}

		if (ImGui::InputText("##AUDIO_SOURCE_COMPONENT_" + a->GetUniqueID(), buf, IM_ARRAYSIZE(buf)))
		{
			a->Event = std::string(buf);
			a->GUID = Chroma::Audio::GetEventGuid(a->Event);
		}


		if (ImGui::Checkbox("##AUDIO_SOURCE_Mute", &a->Mute))
		{

		}


		if (ImGui::Checkbox("##AUDIO_SOURCE_Play on Init", &a->PlayOnInit))
		{

		}

		ImGui::PopItemWidth();
	}

	void ComponentWidgets::DrawBoxCollider(Chroma::Component* c)
	{
		Chroma::BoxCollider* b = reinterpret_cast<Chroma::BoxCollider*>(c);
		DrawComponentValue(c, "Bounds");
		ImGui::Vec2IntWithLabels("##box_collider_2d_bound", b->Bounds);

		DrawComponentValue(c, "Offset");
		ImGui::Vec2IntWithLabels("##box_collider_2d_offset", b->Offset);
	}

	void ComponentWidgets::DrawCircleCollider(Chroma::Component* c)
	{
		Chroma::CircleCollider* b = reinterpret_cast<Chroma::CircleCollider*>(c);
		DrawComponentValue(c, "Radius");
		ImGui::InputFloat("##circle_collider_2d_bounds", &b->Radius);

		DrawComponentValue(c, "Offset");
		ImGui::Vec2IntWithLabels("##transform_rotation", b->Offset);


	}

	void ComponentWidgets::DrawLuaScript(Chroma::Component* c)
	{
		Chroma::LuaScript* script = reinterpret_cast<Chroma::LuaScript*>(c);
		DrawComponentValue(c, "Script");
		const char* preview = script->Path.c_str();
		if (script->Path.empty())
			preview = "[Select a script]";

		if (ImGui::BeginCombo("##script_compbo", preview))
		{
			for (auto& name : Chroma::LuaScripting::Scripts)
			{
				bool selected = name == script->Path;
				if (ImGui::Selectable(name.c_str(), &selected))
				{
					script->ScriptName = std::filesystem::path(name).filename().string();
					script->Path = name;
					script->Success = Chroma::LuaScripting::LoadScriptFromFile(script->Path, script->Environment);
					break;
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}


		if (script->Success)
		{

			script->Environment.for_each([&](const sol::object& key, const sol::object& value) {
				if (!value.is<sol::function>())
				{
					std::string name = key.as<std::string>();
					if (name == "entity" || name == "time")
						return;
					std::string hash = fmt::format("##{}", name);

					DrawComponentValue(c, name);
					if (value.is<bool>())
					{
						bool val = script->Environment[name];
						ImGui::Checkbox(hash.c_str(), &val);
						script->Environment[name] = val;
					}
					else if (value.is<int>())
					{
						int val = script->Environment[name];
						ImGui::InputInt(hash.c_str(), &val);
						script->Environment[name] = val;
					}
					else if (value.is<float>())
					{
						float val = script->Environment[name];
						ImGui::InputFloat(hash.c_str(), &val);
						script->Environment[name] = val;
					}
					else if (value.is<double>())
					{
						double val = script->Environment[name];
						ImGui::InputDouble(hash.c_str(), &val);
						script->Environment[name] = val;
					}
					else if (value.is<std::string>())
					{
						std::string val = script->Environment[name];
						ImGui::InputText(hash.c_str(), &val);
						script->Environment[name] = val;
					}
					else if (value.is<Math::vec2>())
					{
						Math::vec2 val = script->Environment[name];
						ImGui::Vec2FloatWithLabels(hash.c_str(), val, true);
						script->Environment[name] = val;
					}
					else if (value.is<Math::vec3>())
					{
						Math::vec3 val = script->Environment[name];
						ImGui::Vec3FloatWithLabels(hash.c_str(), val, true);
						script->Environment[name] = val;
					}
				}
				});
		}
	}

	void ComponentWidgets::DrawCameraComponent(Chroma::Component* c)
	{
		Chroma::Camera* camera = reinterpret_cast<Chroma::Camera*>(c);

		DrawComponentValue(c, "Size");

		glm::uvec2 camSize = camera->GetSize();
		glm::uvec2 size = { camSize.x, camSize.y };
		ImGui::UVec2IntNoReset("##camera_size", size);

		if (size != camSize)
			camera->SetSize(size.x, size.y);

		DrawComponentValue(c, "PrimaryCamera");
		bool primary = Hierarchy::SelectedEntity == EditorApp::CurrentScene->GetPrimaryCameraEntity();
		if (ImGui::Checkbox("##primary_camera", &primary))
		{
			if (primary)
				EditorApp::CurrentScene->SetPrimaryCamera(Hierarchy::SelectedEntity);
		}

	}


	void ComponentWidgets::DrawSpriteRenderer(Chroma::Component* c)
	{
		Chroma::SpriteRenderer* spr = reinterpret_cast<Chroma::SpriteRenderer*>(c);
		std::string selectedSprite = spr->GetSpriteID();

		DrawComponentValue(c, "Sprite");

		if (ImGui::BeginCombo("##Sprite", spr->GetSpriteID().c_str()))
		{
			for (auto& [id, sprite] : *Chroma::AssetManager::GetSprites())
			{
				bool selected = (spr->GetSpriteID() == sprite->GetPath());
				if (ImGui::Selectable(sprite->GetPath().c_str(), &selected))
				{
					spr->SetSprite(sprite->GetPath());
					spr->SetCurrentFrame(0);
				}

				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}


			}
			ImGui::EndCombo();
		}



		if (Chroma::AssetManager::HasSprite(spr->GetSpriteID()))
		{
			Chroma::Ref<Chroma::Sprite> s = Chroma::AssetManager::GetSprite(spr->GetSpriteID());

			DrawComponentValue(c, "Size");
			ImGui::Text(("[" + std::to_string(s->Frames[spr->GetCurrentFrame()].Texture->GetWidth()) + ", " + std::to_string(s->Frames[spr->GetCurrentFrame()].Texture->GetHeight()) + "]").c_str());

			DrawComponentValue(c, "Origin");
			int selectedOrigin = (int)spr->GetSpriteOrigin();
			const char* const originItems[] = { "Center", "Left", "Right", "Top", "Bottom", "Top Left", "Top Right", "Bottom Left", "Bottom Right", "Custom" };
			if (ImGui::Combo("##origin_dropdown", &selectedOrigin, originItems, 10))
			{
				if (selectedOrigin == (int)Chroma::SpriteRenderer::SpriteOrigin::Custom)
					spr->SetSpriteOrigin(spr->GetSpriteOriginVector());
				else
					spr->SetSpriteOrigin((Chroma::SpriteRenderer::SpriteOrigin)Math::clamp(0, 9, selectedOrigin));
			}

			if ((Chroma::SpriteRenderer::SpriteOrigin)selectedOrigin == Chroma::SpriteRenderer::SpriteOrigin::Custom)
			{
				DrawComponentValue(c, "");
				Math::vec2 customOrigin = spr->GetSpriteOriginVector();
				if (ImGui::Vec2IntWithLabels("##spriteRendererCustomOrigin", customOrigin))
				{
					spr->SetSpriteOrigin(customOrigin);
				}
			}


			if (s->Animated())
			{
				ImGui::Dummy({ 1,1 });
				ImGui::Separator();
				ImGui::Dummy({ 1,1 });
				DrawComponentValue(c, "Animation");
				std::string selectedStr;
				if (spr->GetAnimation() > s->Animations.size())
					selectedStr = s->Animations[0].Tag;
				else
					selectedStr = s->Animations[spr->GetAnimation()].Tag;

				if (ImGui::BeginCombo("##animation", selectedStr.c_str()))
				{
					int i = 0;
					for (Chroma::Sprite::Animation anim : s->Animations)
					{
						bool selected = (spr->GetAnimation() == i);
						if (ImGui::Selectable(anim.Tag.c_str(), &selected))
							spr->SetAnimation(i);
						if (selected)
						{
							ImGui::SetItemDefaultFocus();
						}

						i++;
					}
					ImGui::EndCombo();
				}

				if (spr->GetAnimation() >= 0 && spr->GetAnimation() < s->Animations.size())
				{
					DrawComponentValue(c, "");
					std::string frms = std::to_string(s->Animations[spr->GetAnimation()].Start + 1) + " - " + std::to_string(s->Animations[spr->GetAnimation()].End + 1);
					Chroma::Sprite::LoopDirection direction = s->Animations[spr->GetAnimation()].Direction;
					std::string res;
					if (direction == Chroma::Sprite::LoopDirection::Forward)
						res = "Forward";
					else if (direction == Chroma::Sprite::LoopDirection::PingPong)
						res = "Ping Pong";
					else
						res = "Reverse";

					ImGui::Text((frms + " " + res).c_str());
				}


			}

			if (s->Frames.size() > 1)
			{
				DrawComponentValue(c, "Frame");

				int curr = (int)spr->GetCurrentFrame() + 1;
				ImGui::SliderInt("##frame", &curr, 1, s->Frames.size());
				spr->SetCurrentFrame((unsigned int)curr - 1);


				ImGui::SameLine();
				ImGui::Text((std::to_string(s->Frames[spr->GetCurrentFrame()].Durration) + " ms").c_str());
			}


			if (s->Animated())
			{
				DrawComponentValue(c, "Loop");
				ImGui::Checkbox("##loop", &spr->Loop);

				DrawComponentValue(c, "Play On Start");
				ImGui::Checkbox("##play_on_start", &spr->PlayOnStart);

				DrawComponentValue(c, "Playing");
				ImGui::Checkbox("##playing", &spr->Playing);
			}


		}









		DrawComponentValue(c, "Color");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		float col[4]{ spr->Color.r, spr->Color.b, spr->Color.g, spr->Color.a };
		if (ImGui::ColorEdit4("##Color", col))
		{
			spr->Color = { col[0], col[1], col[2], col[3] };
		}


		DrawComponentValue(c, "Offset");
		ImGui::Vec2IntWithLabels("##offset", spr->Offset);


		//DrawComponentValue("Layer");

		//static bool editing = false;

		/*
		if (!editing && ImGui::BeginCombo("##Layer", Layer.c_str()))
		{
			for (std::string name : Application::Get().m_ActiveScene->Layers)
			{
				bool isSelected = (name == Layer);
				if (ImGui::Selectable(name.c_str(), isSelected))
					Layer = name;

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine();

		if (std::find(Application::Get().m_ActiveScene->Layers.begin(), Application::Get().m_ActiveScene->Layers.end(), "Default") == Application::Get().m_ActiveScene->Layers.end())
		{
			Application::Get().m_ActiveScene->Layers.push_back("Default");
		}

		if (std::find(Application::Get().m_ActiveScene->Layers.begin(), Application::Get().m_ActiveScene->Layers.end(), Layer) == Application::Get().m_ActiveScene->Layers.end())
		{
			Layer = "Default";
		}


		if (!editing && ImGui::Button(ICON_FK_COG))
		{
			editing = true;
		}

		if (editing)
		{
			DrawComponentValue("    " ICON_FK_ANGLE_DOUBLE_RIGHT);
			ImGui::EditableList(Application::Get().m_ActiveScene->Layers, true);
			if (ImGui::Button(ICON_FK_CHECK " Accept"))
			{
				editing = false;
			}
		}

		*/


		//bool open = DrawComponentValueCollapsible("Layer");
		//ImGui::EditableList(this->ListTest, open);


	}

	void ComponentWidgets::DrawComponentValue(Chroma::Component* c, const std::string& label)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		if (show_context)
		{
			context_function(label, c->comparison_id);
		}

		ImGui::TableSetColumnIndex(1);
	}

	bool ComponentWidgets::DrawComponentValueCollapsible(Chroma::Component* c, const std::string& label)
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
			context_function(label, c->comparison_id);
		}

		ImGui::TableSetColumnIndex(1);
		ImGui::NewLine();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		ImGui::PopID();

		return collapsibleOpen[id];

	}
}