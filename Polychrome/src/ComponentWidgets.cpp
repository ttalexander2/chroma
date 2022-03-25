#include "ComponentWidgets.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include "Fonts/IconsForkAwesome.h"
#include <Chroma/Components/AudioSource.h>
#include <Chroma/Audio/Audio.h>
#include <Chroma/Components/Transform.h>
#include "Utilities/VecWithLabels.h"
#include <Chroma/Components/CSharpScript.h>
#include <Chroma/Scripting/ScriptModuleField.h>
#include <Chroma/Components/BoxCollider.h>
#include <Chroma/Components/CircleCollider.h>
#include <Chroma/Scripting/MonoScripting.h>
#include <Chroma/Components/SpriteRenderer.h>
#include <Chroma/Components/ParticleEmitter.h>
#include <Chroma/Assets/AssetManager.h>
#include "AssetBrowser.h"

#include "EditorApp.h"
#include "Hierarchy.h"
#include "UndoRedo.h"
#include "Project.h"
#include "Viewport.h"
#include "ComponentDebugGizmos.h"


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
		if (c->IsTypeOf<Chroma::Transform>()) DrawTransform(c);
		if (c->IsTypeOf<Chroma::CSharpScript>()) DrawCSharpScript(c);
		if (c->IsTypeOf<Chroma::AudioSource>()) DrawAudioSource(c);
		if (c->IsTypeOf<Chroma::BoxCollider>()) DrawBoxCollider(c);
		if (c->IsTypeOf<Chroma::CircleCollider>()) DrawCircleCollider(c);
		if (c->IsTypeOf<Chroma::SpriteRenderer>()) DrawSpriteRenderer(c);
		if (c->IsTypeOf<Chroma::Camera>()) DrawCameraComponent(c);
		if (c->IsTypeOf<Chroma::ParticleEmitter>()) DrawParticleEmitter(c);
	}

	void ComponentWidgets::DrawTransform(Chroma::Component* c)
	{
		Chroma::Transform* t = reinterpret_cast<Chroma::Transform*>(c);
		DrawComponentValue(t, "Position");
		ImGui::Vec2IntWithLabels(("##transform_position" + std::to_string(t->GetUniqueID())).c_str(), t->Position);

		float oldRot = t->Rotation;

		DrawComponentValue(t, "Rotation");
		float rot = glm::degrees(t->Rotation);
		ImGui::SetNextItemWidth(ImGui::GetColumnWidth(2) - 2);
		if (ImGui::SliderFloat(("##transform_rotation" + std::to_string(t->GetUniqueID())).c_str(), &rot, 0.f, 360.f))
		{
			t->Rotation = glm::radians(rot);
		}

		Polychrome::UndoRedo::ImGuiRegister<float>(&t->Rotation, oldRot, "Modify Transform Rotation");
		

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
				std::string hash = "##" + script->ModuleName + name;
				if (field.Type == Chroma::FieldType::Float)
				{
					float val = field.GetRuntimeValue<float>(entityInstanceData.Instance);
					ImGui::InputFloat(hash.c_str(), &val);
					field.SetRuntimeValue<float>(entityInstanceData.Instance, val);
				}
				else if (field.Type == Chroma::FieldType::Int)
				{
					int val = field.GetRuntimeValue<int>(entityInstanceData.Instance);
					ImGui::InputInt(hash.c_str(), &val);
					field.SetRuntimeValue<int>(entityInstanceData.Instance, val);
				}
				else if (field.Type == Chroma::FieldType::String)
				{
					std::string val = field.GetRuntimeValue<std::string>(entityInstanceData.Instance);
					ImGui::InputText(hash.c_str(), &val);
					field.SetRuntimeValue(entityInstanceData.Instance, val);
				}
				else if (field.Type == Chroma::FieldType::Entity)
				{
					//Chroma::Entity entity = field.GetRuntimeValue<Chroma::Entity>(entityInstanceData.Instance);
					//int id = (int)entity.GetID();
					//ImGui::InputInt(hash, &id, 1, 100);
					//if (EditorApp::CurrentScene->Registry.valid((Chroma::EntityID)id))
					//	field.SetRuntimeValue<Chroma::Entity>(entityInstanceData.Instance, Chroma::Entity((Chroma::EntityID)id, EditorApp::CurrentScene));
				}
			}
		}
		else
		{
			for (auto& [name, field] : script->ModuleFieldMap[script->ModuleName])
			{
				DrawComponentValue(script, name);
				std::string hash = "##" + script->ModuleName + name;
				if (field.Type == Chroma::FieldType::Float)
				{
					float val = field.GetStoredValue<float>();
					ImGui::InputFloat(hash.c_str(), &val);
					field.SetStoredValue<float>(val);
				}
				else if (field.Type == Chroma::FieldType::Int)
				{
					int val = field.GetStoredValue<int>();
					ImGui::InputInt(hash.c_str(), &val);
					field.SetStoredValue<int>(val);
				}
				else if (field.Type == Chroma::FieldType::String)
				{
					std::string val = field.GetStoredValue<const std::string&>();
					ImGui::InputText(hash.c_str(), &val);
					field.SetStoredValue<const std::string&>(val);
				}
				else if (field.Type == Chroma::FieldType::Entity)
				{
					//Chroma::Entity entity = field.GetStoredValue<Chroma::Entity>();
					//int id = (int)entity.GetID();
					//ImGui::InputInt(hash, &id, 1, 100);
					//if (EditorApp::CurrentScene->Registry.valid((Chroma::EntityID)id))
					//	field.SetStoredValue<Chroma::Entity>(Chroma::Entity((Chroma::EntityID)id, EditorApp::CurrentScene));

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

		std::string evnt = a->GetEvent();
		if (ImGui::InputText("##AUDIO_SOURCE_COMPONENT_" + a->GetUniqueID(), buf, IM_ARRAYSIZE(buf)))
		{
			a->SetEvent(std::string(buf));
		}

		Polychrome::UndoRedo::ImGuiRegister<std::string>(&a->Event, evnt, "Modify AudioSource Event");


		bool mute = a->Mute;
		ImGui::Checkbox("##AUDIO_SOURCE_Mute", &a->Mute);
		Polychrome::UndoRedo::ImGuiRegister<bool>(&a->Mute, mute, "Modify AudioSource Mute");


		bool pon = a->PlayOnInit;
		ImGui::Checkbox("##AUDIO_SOURCE_Play on Init", &a->PlayOnInit);
		Polychrome::UndoRedo::ImGuiRegister<bool>(&a->Mute, pon, "Modify AudioSource PlayOnInit");

		ImGui::PopItemWidth();
	}

	void ComponentWidgets::DrawBoxCollider(Chroma::Component* c)
	{
		Chroma::BoxCollider* b = reinterpret_cast<Chroma::BoxCollider*>(c);

		std::vector<std::string> names;
		for (int i = 0; i < 32; i++)
		{
			names.push_back("Layer " + std::to_string(i + 1));
		}

		if (DrawComponentValueCollapsible(c, "Collision Layer"))
		{
			if (ImGui::BeginListBox("##layer_list_box", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (int i = 0; i < 32; i++)
				{
					bool is_selected = (b->m_Layer >> i) & 1U;
					if (ImGui::Selectable(names[i].c_str(), &is_selected))
					{
						b->m_Layer ^= 1UL << i;
					}
				}
				ImGui::EndListBox();
			}
		}

		if (DrawComponentValueCollapsible(c, "Collision Mask"))
		{
			if (ImGui::BeginListBox("##mask_list_box", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (int i = 0; i < 32; i++)
				{
					bool is_selected = (b->m_Mask >> i) & 1U;
					if (ImGui::Selectable(names[i].c_str(), &is_selected))
					{
						b->m_Mask ^= 1UL << i;
					}
				}
				ImGui::EndListBox();
			}
		}



		DrawComponentValue(c, "Min");
		{
			std::string hash = "box_collider_min";

			bool changed = false;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

			float inputWidth = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("X").x * 2.0f - (4.0f * 6)) / 2;

			Math::vec2& val = b->Min;

			Math::vec2 oldVal = val;

			ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.1f, 0.8f });
			if (ImGui::Button(("X##vec3_x_label" + std::string(hash)).c_str()))
			{
				changed = true;
				val.x = 1;
			}

			Polychrome::UndoRedo::ImGuiRegister<Math::vec2>(&val, oldVal, "Reset Vec2.X");

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Click to reset value.");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();

			ImGui::SetNextItemWidth(inputWidth);

			int pos = (int)val.x;
			if (ImGui::DragInt(("##vec3_x" + std::string(hash)).c_str(), &pos))
			{
				changed = true;
				val.x = pos;
			}

			Polychrome::UndoRedo::ImGuiRegister<Math::vec2>(&val, oldVal, "Modify Vec2.X");

			ImGui::SameLine(0.0f, 6.0f);

			ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.8f, 0.1f, 0.8f });
			if (ImGui::Button(("Y##vec3_y_label" + std::string(hash)).c_str()))
			{
				changed = true;
				val.y = 1;
			}

			Polychrome::UndoRedo::ImGuiRegister<Math::vec2>(&val, oldVal, "Reset Vec2.Y");

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Click to reset value.");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();

			ImGui::SetNextItemWidth(inputWidth);

			pos = val.y;
			if (ImGui::DragInt(("##vec3_y" + std::string(hash)).c_str(), &pos))
			{
				changed = true;
				val.y = pos;
			}

			Polychrome::UndoRedo::ImGuiRegister<Math::vec2>(&val, oldVal, "Modify Vec2.Y");

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, 6.0f);
			ImGui::NewLine();
		}

		


		//MAX

		DrawComponentValue(c, "Max");

		{
			std::string hash = "box_collider_max";

			bool changed = false;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

			float inputWidth = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("X").x * 2.0f - (4.0f * 6)) / 2;

			Math::vec2& val = b->Max;

			Math::vec2 oldVal = val;

			ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.1f, 0.8f });
			if (ImGui::Button(("X##vec3_x_label" + std::string(hash)).c_str()))
			{
				changed = true;
				val.x = 1;
			}

			Polychrome::UndoRedo::ImGuiRegister<Math::vec2>(&val, oldVal, "Reset Vec2.X");

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Click to reset value.");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();

			ImGui::SetNextItemWidth(inputWidth);

			int pos = (int)val.x;
			if (ImGui::DragInt(("##vec3_x" + std::string(hash)).c_str(), &pos))
			{
				changed = true;
				val.x = pos;
			}

			Polychrome::UndoRedo::ImGuiRegister<Math::vec2>(&val, oldVal, "Modify Vec2.X");

			ImGui::SameLine(0.0f, 6.0f);

			ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.8f, 0.1f, 0.8f });
			if (ImGui::Button(("Y##vec3_y_label" + std::string(hash)).c_str()))
			{
				changed = true;
				val.y = 1;
			}

			Polychrome::UndoRedo::ImGuiRegister<Math::vec2>(&val, oldVal, "Reset Vec2.Y");

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Click to reset value.");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();

			ImGui::SetNextItemWidth(inputWidth);

			pos = val.y;
			if (ImGui::DragInt(("##vec3_y" + std::string(hash)).c_str(), &pos))
			{
				changed = true;
				val.y = pos;
			}

			Polychrome::UndoRedo::ImGuiRegister<Math::vec2>(&val, oldVal, "Modify Vec2.Y");

			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, 6.0f);
			ImGui::NewLine();
		}

		
		
	}

	void ComponentWidgets::DrawCircleCollider(Chroma::Component* c)
	{
		Chroma::CircleCollider* b = reinterpret_cast<Chroma::CircleCollider*>(c);
		DrawComponentValue(c, "Radius");
		ImGui::InputFloat("##circle_collider_2d_bounds", &b->Radius);

		DrawComponentValue(c, "Offset");
		ImGui::Vec2IntWithLabels("##transform_rotation", b->Offset);


	}

	void ComponentWidgets::DrawCameraComponent(Chroma::Component* c)
	{
		Chroma::Camera* camera = reinterpret_cast<Chroma::Camera*>(c);

		DrawComponentValue(c, "Size");

		glm::uvec2 camSize = camera->GetSize();
		glm::uvec2 size = { camSize.x, camSize.y };
		ImGui::UVec2IntNoReset("##camera_size", size);

		if (size != camSize)
			camera->SetSize({ size.x, size.y });

		DrawComponentValue(c, "PrimaryCamera");
		bool primary = Hierarchy::SelectedEntity == EditorApp::CurrentScene->GetPrimaryCameraEntity();
		if (ImGui::Checkbox("##primary_camera", &primary))
		{
			if (primary)
				EditorApp::CurrentScene->SetPrimaryCamera(Hierarchy::SelectedEntity);
		}

	}

	void ComponentWidgets::DrawParticleEmitter(Chroma::Component* c)
	{
		Chroma::ParticleEmitter* emitter = reinterpret_cast<Chroma::ParticleEmitter*>(c);

		{
			DrawComponentValue(c, "Count");
			int count = emitter->GetCount();
			if (ImGui::DragInt("##pecounttt", &count, 1.f, 0.f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp))
			{
				emitter->SetCount((size_t)count);
			}

		}

		DrawComponentValue(c, "Position Variance");
		Math::vec2 oldPosVar = emitter->PositionVariance;
		ImGui::Vec2IntWithLabels("##peposvar", emitter->PositionVariance);
		//UndoRedo::ImGuiRegister<Math::vec2>(&emitter->PositionVariance, oldPosVar, "Modify Particle Emitter Position Variance");

		DrawComponentValue(c, "Start Color");
		
		//ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		Math::vec4 oldColor = emitter->StartColor;
		float col[4]{ emitter->StartColor.r, emitter->StartColor.g, emitter->StartColor.b, emitter->StartColor.a };
		if (ImGui::ColorEdit4("##Color", col))
		{
			emitter->StartColor = { col[0], col[1], col[2], col[3] };
		}
		UndoRedo::ImGuiRegister<Math::vec4>(&emitter->StartColor, oldColor, "Modify Particle Emitter Start Color");

		DrawComponentValue(c, "End Color");

		//ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		oldColor = emitter->EndColor;
		float col2[4]{ emitter->EndColor.r, emitter->EndColor.g, emitter->EndColor.b, emitter->EndColor.a };
		if (ImGui::ColorEdit4("##Color1", col2))
		{
			emitter->EndColor = { col2[0], col2[1], col2[2], col2[3] };
		}
		UndoRedo::ImGuiRegister<Math::vec4>(&emitter->EndColor, oldColor, "Modify Particle Emitter End Color");

		DrawComponentValue(c, "Life");
		float oldLife = emitter->Life;
		ImGui::DragFloat("##pelife", &emitter->Life, 1.f, 0.f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);
	
		UndoRedo::ImGuiRegister<float>(&emitter->Life, oldLife, "Modify Particle Emitter Life");

		{
			DrawComponentValue(c, "Life Variance");
			float oldLifevar = emitter->LifeVariance;
			ImGui::DragFloat("##pelifevar", &emitter->LifeVariance, 1.f, 0.f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);

			UndoRedo::ImGuiRegister<float>(&emitter->LifeVariance, oldLifevar, "Modify Particle Emitter Life Variance");
		}

		{
			DrawComponentValue(c, "Angle");
			float oldAngle = emitter->Angle;
			ImGui::DragFloat("##pelifeAngle", &emitter->Angle, 1.f, 0.f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);

			UndoRedo::ImGuiRegister<float>(&emitter->Angle, oldAngle, "Modify Particle Emitter Angle");
		}

		{
			DrawComponentValue(c, "Angle Variance");
			float oldAngleVariance = emitter->AngleVariance;
			ImGui::DragFloat("##pelifeAngleVariance", &emitter->AngleVariance, 1.f, 0.f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);

			UndoRedo::ImGuiRegister<float>(&emitter->AngleVariance, oldAngleVariance, "Modify Particle Emitter Angle Variance");
		}

		{
			DrawComponentValue(c, "Speed");
			float oldSpeed = emitter->Speed;
			ImGui::DragFloat("##pelifeSpeed", &emitter->Speed, 1.f, 0.f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);

			UndoRedo::ImGuiRegister<float>(&emitter->Speed, oldSpeed, "Modify Particle Emitter Speed");
		}

		{
			DrawComponentValue(c, "Speed Variance");
			float oldSpeedVariance = emitter->SpeedVariance;
			ImGui::DragFloat("##pelifeSpeedVar", &emitter->SpeedVariance, 1.f, 0.f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);

			UndoRedo::ImGuiRegister<float>(&emitter->SpeedVariance, oldSpeedVariance, "Modify Particle Emitter Speed Variance");
		}

		{
			DrawComponentValue(c, "Gravity");
			float oldGravity = emitter->Gravity;
			ImGui::DragFloat("##pelifeSGravity", &emitter->Gravity, 1.f, std::numeric_limits<float>::min(), std::numeric_limits<float>::max());

			UndoRedo::ImGuiRegister<float>(&emitter->Gravity, oldGravity, "Modify Particle Emitter Gravity");
		}

		{
			DrawComponentValue(c, "Gravity Variance");
			float oldGravityVariance = emitter->GravityVariance;
			ImGui::DragFloat("##pelifeSGravityVar", &emitter->GravityVariance, 1.f, std::numeric_limits<float>::min(), std::numeric_limits<float>::max());

			UndoRedo::ImGuiRegister<float>(&emitter->GravityVariance, oldGravityVariance, "Modify Particle Emitter Gravity Variance");
		}

		{
			DrawComponentValue(c, "Continuous");
			bool oldContinuous = emitter->Continuous;
			ImGui::Checkbox("##pelifeContinuous", &emitter->Continuous);

			UndoRedo::ImGuiRegister<bool>(&emitter->Continuous, oldContinuous, "Modify Particle Emitter Settings");
		}

		{
			DrawComponentValue(c, "Emission Rate");
			float oldEmissionRate = emitter->EmissionRate;
			ImGui::DragFloat("##pelifeEmissionRate", &emitter->EmissionRate, 1.f, 0.f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);

			UndoRedo::ImGuiRegister<float>(&emitter->EmissionRate, oldEmissionRate, "Modify Particle Emitter Emission Rate");
		}

		{
			DrawComponentValue(c, "Pre-warm");
			bool oldPreWarm = emitter->PreWarm;
			ImGui::Checkbox("##pelifePreWarms", &emitter->PreWarm);

			UndoRedo::ImGuiRegister<bool>(&emitter->PreWarm, oldPreWarm, "Modify Particle Emitter Settings");
		}

		{
			DrawComponentValue(c, "Pre-warm Seconds");
			float oldPreWarmSeconds = emitter->PreWarmSeconds;
			ImGui::DragFloat("##pelifePreWarmSeconds", &emitter->PreWarmSeconds, 1.f, 0.f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);

			UndoRedo::ImGuiRegister<float>(&emitter->PreWarmSeconds, oldPreWarmSeconds, "Modify Particle Emitter Settings");
		}

	}


	void ComponentWidgets::DrawSpriteRenderer(Chroma::Component* c)
	{
		Chroma::SpriteRenderer* spr = reinterpret_cast<Chroma::SpriteRenderer*>(c);
		auto selectedSprite = spr->GetSpritePath();

		DrawComponentValue(c, "Sprite");

		std::string spr_path = spr->GetSpritePath();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
		if (ImGui::Button(ICON_FK_LIST "##sprite_selection_button"))
		{
			//Chroma::EntityID* id = new Chroma::EntityID(spr->GetEntityID());
			AssetBrowser::SelectAsset<Chroma::Sprite>([](Chroma::Ref<Chroma::Asset> asset, void* user_data) {
				Chroma::SpriteRenderer* e = reinterpret_cast<Chroma::SpriteRenderer*>(user_data);
				e->SetSprite(asset->GetID());
				//delete e;
				}, (void*)spr);
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Select Asset");

		ImGui::SameLine();
		ImGui::InputText("##sprite_to_render", &spr_path, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopStyleVar();

		/*
		if (ImGui::BeginCombo("##Sprite", spr->GetSpritePath().c_str()))
		{
			for (auto sprite : Chroma::AssetManager::View<Chroma::Sprite>())
			{
				bool selected = (spr->GetSpritePath() == sprite->GetPath());
				if (ImGui::Selectable(sprite->GetPath().c_str(), &selected))
				{
					spr->SetSprite(sprite->GetID());
					spr->SetCurrentFrame(0);
				}

				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}


			}
			ImGui::EndCombo();
		}
		*/




		if (Chroma::AssetManager::Exists(spr->GetSpriteID()))
		{
			Chroma::Ref<Chroma::Sprite> s = Chroma::AssetManager::Get<Chroma::Sprite>(spr->GetSpriteID());

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
			
			float oldSortingPoint = spr->SortingPoint;
			DrawComponentValue(c, "Sorting Point");
			static bool dragging = false;
			static bool gizmo_on = false;
			ImGui::DragFloat("##sorting_point_sprite", &spr->SortingPoint);

			if (ImGui::IsItemActive())
			{
				
				if (!dragging)
				{
					gizmo_on = ComponentDebugGizmos::DrawSpriteBoundries;
					ComponentDebugGizmos::DrawSpriteBoundries = true;
				}
					
				dragging = true;
			}
			else if(dragging)
			{
				dragging = false;
				if (!gizmo_on)
					ComponentDebugGizmos::DrawSpriteBoundries = false;
			}

			UndoRedo::ImGuiRegister<float>(&spr->SortingPoint, oldSortingPoint, "Modify Sprite Sorting Point");


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

				unsigned int oldAnimation = spr->GetAnimation();

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

				UndoRedo::ImGuiRegister<unsigned int>(&spr->Animation, oldAnimation, "Modify Sprite Animation");

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
				//unsigned int oldFrame = spr->CurrentFrame;
				int curr = (int)spr->GetCurrentFrame() + 1;
				ImGui::SliderInt("##frame", &curr, 1, s->Frames.size());
				spr->SetCurrentFrame((unsigned int)curr - 1);
				//UndoRedo::ImGuiRegister<unsigned int>(&spr->CurrentFrame, oldFrame, "Modify Sprite Frame");

				ImGui::SameLine();

				if (ImGui::Button("<"))
				{
					spr->SetCurrentFrame(((unsigned int)curr - 2) % s->Frames.size());
				}

				ImGui::SameLine();
				if (ImGui::Button(">"))
				{
					spr->SetCurrentFrame(((unsigned int)curr) % s->Frames.size());
				}


				ImGui::SameLine();
				ImGui::Text((std::to_string(s->Frames[spr->GetCurrentFrame()].Durration) + " ms").c_str());
			}


			if (s->Animated())
			{
				bool oldLoop = spr->Loop;
				DrawComponentValue(c, "Loop");
				ImGui::Checkbox("##loop", &spr->Loop);
				UndoRedo::ImGuiRegister<bool>(&spr->Loop, oldLoop, "Modify Sprite Loop");

				bool oldpos = spr->PlayOnStart;
				DrawComponentValue(c, "Play On Start");
				ImGui::Checkbox("##play_on_start", &spr->PlayOnStart);
				UndoRedo::ImGuiRegister<bool>(&spr->PlayOnStart, oldpos, "Modify Sprite PlayOnStart");

				bool oldPlaying = spr->Playing;
				DrawComponentValue(c, "Playing");
				ImGui::Checkbox("##playing", &spr->Playing);
				UndoRedo::ImGuiRegister<bool>(&spr->Playing, oldPlaying, "Modify Sprite Playing");
			}


		}









		DrawComponentValue(c, "Color");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		Math::vec4 oldColor = spr->Color;

		float col[4]{ spr->Color.r, spr->Color.g, spr->Color.b, spr->Color.a };
		if (ImGui::ColorEdit4("##Color", col))
		{
			spr->Color = { col[0], col[1], col[2], col[3] };
		}

		UndoRedo::ImGuiRegister<Math::vec4>(&spr->Color, oldColor, "Modify Sprite Color");


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