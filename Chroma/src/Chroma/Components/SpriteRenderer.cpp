#include "chromapch.h"
#include "SpriteRenderer.h"
#include "Chroma/ImGui/Widgets/VecWithLabels.h"
#include "Chroma/ImGui/Widgets/EditableList.h"
#include "Chroma/ImGui/Widgets/AlternateCheckBox.h"
#include "imgui_internal.h"
#include "Chroma/Core/Application.h"
#include "Chroma/Assets/AssetManager.h"

namespace Chroma
{
	
	void SpriteRenderer::DrawImGui()
	{

		std::string selectedSprite = SpriteID;

		DrawComponentValue("Sprite");

		if (ImGui::BeginCombo("##Sprite", SpriteID.c_str()))
		{
			for (auto& [id, sprite] : *AssetManager::GetSprites())
			{
				bool selected = (SpriteID == sprite->GetPath());
				if (ImGui::Selectable(sprite->GetPath().c_str(), &selected))
				{
					SetSprite(sprite->GetPath());
					SetCurrentFrame(0);
				}
					
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
					

			}
			ImGui::EndCombo();
		}

		

		if (AssetManager::HasSprite(SpriteID))
		{
			Ref<Sprite> s = AssetManager::GetSprite(SpriteID);

			DrawComponentValue("Size");
			ImGui::Text(("[" + std::to_string(s->Frames[CurrentFrame].Texture->GetWidth()) + ", " + std::to_string(s->Frames[CurrentFrame].Texture->GetHeight()) + "]").c_str());

			if (s->Animated())
			{
				DrawComponentValue("Animation");
				std::string selectedStr;
				if (Animation > s->Animations.size())
					selectedStr = s->Animations[0].Tag;
				else
					selectedStr = s->Animations[Animation].Tag;

				if (ImGui::BeginCombo("##animation", selectedStr.c_str()))
				{
					int i = 0;
					for (Sprite::Animation anim : s->Animations)
					{
						bool selected = (Animation == i);
						if (ImGui::Selectable(anim.Tag.c_str(), &selected))
							SetAnimation(i);
						if (selected)
						{
							ImGui::SetItemDefaultFocus();
						}

						i++;
					}
					ImGui::EndCombo();
				}

				if (Animation >= 0 && Animation < s->Animations.size())
				{
					DrawComponentValue("");
					std::string frms = std::to_string(s->Animations[Animation].Start + 1) + " - " + std::to_string(s->Animations[Animation].End + 1);
					Sprite::LoopDirection direction = s->Animations[Animation].Direction;
					std::string res;
					if (direction == Sprite::LoopDirection::Forward)
						res = "Forward";
					else if (direction == Sprite::LoopDirection::PingPong)
						res = "Ping Pong";
					else
						res = "Reverse";

					ImGui::Text((frms + " " + res).c_str());
				}
		

			}

			if (s->Frames.size() > 1)
			{
				DrawComponentValue("Frame");

				int curr = (int)CurrentFrame + 1;
				ImGui::SliderInt("##frame", &curr, 1, s->Frames.size());
				SetCurrentFrame((unsigned int)curr - 1);


				ImGui::SameLine();
				ImGui::Text((std::to_string(s->Frames[CurrentFrame].Durration) + " ms").c_str());
			}


			if (s->Animated())
			{
				DrawComponentValue("Loop");
				ImGui::Checkbox("##loop", &Loop);

				DrawComponentValue("Play On Start");
				ImGui::Checkbox("##play_on_start", &PlayOnStart);

				DrawComponentValue("Playing");
				ImGui::Checkbox("##playing", &Playing);
			}


		}
		






		

		DrawComponentValue("Color");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		float col[4]{ Color.r, Color.b, Color.g, Color.a };
		if (ImGui::ColorEdit4("##Color", col))
		{
			Color = { col[0], col[1], col[2], col[3] };
		}

		
		DrawComponentValue("Offset");
		ImGui::Vec3IntWithLabels("##offset", Offset);


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

	void SpriteRenderer::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Color";
		out << YAML::Value << Color;

		out << YAML::Key << "Offset";
		out << YAML::Value << Offset;

		out << YAML::Key << "Layer";
		out << YAML::Value << Layer;

		out << YAML::Key << "PlayOnStart";
		out << YAML::Value << PlayOnStart;

		out << YAML::Key << "SpriteID";
		out << YAML::Value << SpriteID;

		out << YAML::Key << "Loop";
		out << YAML::Value << Loop;

		out << YAML::Key << "Animation";
		out << YAML::Value << Animation;


	}
	void SpriteRenderer::Deserialize(YAML::Node& node)
	{
		auto val = node["Color"];
		if (val)
		{
			Color = val.as<Math::vec4>();
		}
		val = node["Offset"];
		if (val)
		{
			Offset = val.as<Math::vec3>();
		}
		val = node["Layer"];
		if (val)
		{
			Layer = val.as<std::string>();
		}
		val = node["PlayOnStart"];
		if (val)
		{
			PlayOnStart = val.as<bool>();
		}
		val = node["SpriteID"];
		if (val)
		{
			SpriteID = val.as<std::string>();
		}
		val = node["Loop"];
		if (val)
		{
			Loop = val.as<bool>();
		}
		val = node["Animation"];
		if (val)
		{
			Animation = val.as<unsigned int>();
		}

	}

	/// @brief Set the sprite to render.
	/// 
	/// Sprite must already be loaded.
	/// @see AssetManager to load sprite.
	/// @param spriteID Name/Path of the sprite.
	void SpriteRenderer::SetSprite(const std::string& spriteID)
	{
		if (AssetManager::HasSprite(spriteID))
		{
			SpriteID = spriteID;
			Animation = 0;
			CurrentFrame = 0;
		}
			
		else
			CHROMA_CORE_WARN("Attempted to set sprite. Sprite [{}] could not be found or is not loaded.", spriteID);
	}

	void SpriteRenderer::SetAnimation(unsigned int animation)
	{
		if (AssetManager::HasSprite(SpriteID))
		{
			Ref<Sprite> s = AssetManager::GetSprite(SpriteID);
			if (animation >= s->Animations.size())
			{
				CHROMA_CORE_WARN("Attempted to set sprite animation number. Sprite [{}] does not contain animation [{}].", SpriteID, animation);
				Animation = s->Animations.size() - 1;
				return;
			}
			bool restart = Animation != animation;
			Animation = animation;
			if (restart)
				RestartAnimation();
			
		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite animation. Sprite [{}] could not be found or is not loaded.", SpriteID);
		}
			
	}

	void SpriteRenderer::SetAnimation(const std::string& animation_name)
	{
		if (AssetManager::HasSprite(SpriteID))
		{
			Ref<Sprite> s = AssetManager::GetSprite(SpriteID);
			bool exists = false;
			unsigned int i = 0;
			for (auto& anim : s->Animations)
			{
				if (anim.Tag == animation_name)
				{
					exists = true;
					bool restart = Animation != i;
					Animation = i;
					if (restart)
						RestartAnimation();
				}
				i++;
			}
			if (!exists)
			{
				CHROMA_CORE_WARN("Attempted to set sprite animation. Sprite [{}] does not contain animation [{}]", SpriteID, animation_name);
			}
		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite animation. Sprite [{}] could not be found or is not loaded.", SpriteID);
		}
	}

	int SpriteRenderer::GetAnimation()
	{
		return Animation;
	}

	std::string SpriteRenderer::GetAnimationName(unsigned int animation)
	{
		if (AssetManager::HasSprite(SpriteID))
		{
			Ref<Sprite> s = AssetManager::GetSprite(SpriteID);
			if (animation >= s->Animations.size())
				CHROMA_CORE_WARN("Attempted to get sprite animation name. Sprite [{}] does not contain animation [{}]", SpriteID, animation);
			else
				return s->Animations[animation].Tag;
			
		}
		return "";
	}

	int SpriteRenderer::GetCurrentFrame()
	{
		return CurrentFrame;
	}

	void SpriteRenderer::SetCurrentFrame(unsigned int frame)
	{
		if (frame == 0)
			CurrentFrame = 0;

		if (AssetManager::HasSprite(SpriteID))
		{
			Ref<Sprite> s = AssetManager::GetSprite(SpriteID);
			if (frame >= s->Frames.size())
				CHROMA_CORE_WARN("Attempted to set sprite frame. Frame [{}] is an invalid frame for Sprite [{}].", frame, SpriteID);
			else
				CurrentFrame = frame;

		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite frame. Sprite [{}] could not be found or is not loaded.", SpriteID);
		}

		
	}

	void SpriteRenderer::RestartAnimation()
	{
		if (AssetManager::HasSprite(SpriteID) && AssetManager::GetSprite(SpriteID)->Animated())
		{
			Ref<Sprite> s = AssetManager::GetSprite(SpriteID);
			if (Animation >= s->Animations.size())
				Animation = s->Animations.size() - 1;
			else if (Animation < 0)
				Animation = 0;
			CurrentFrame = s->Animations[Animation].Start;
		}
	}

}


