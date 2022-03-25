#include "chromapch.h"
#include "SpriteRenderer.h"
#include "Chroma/Core/Application.h"
#include "Chroma/Assets/AssetManager.h"

#include <functional>

namespace Chroma
{
	
	void SpriteRenderer::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Color";
		out << YAML::Value << Color;

		out << YAML::Key << "Offset";
		out << YAML::Value << Offset;

		out << YAML::Key << "PlayOnStart";
		out << YAML::Value << PlayOnStart;

		out << YAML::Key << "SpriteID";
		out << YAML::Value << SpriteID.ToString();

		out << YAML::Key << "Loop";
		out << YAML::Value << Loop;

		out << YAML::Key << "Animation";
		out << YAML::Value << Animation;

		if (Origin == SpriteOrigin::Custom)
		{
			out << YAML::Key << "CustomOrigin";
			out << YAML::Value << OriginValue;
		}
		else
		{
			out << YAML::Key << "Origin";
			out << YAML::Value << (int)Origin;
		}

		out << YAML::Key << "SortingPoint";
		out << YAML::Value << SortingPoint;


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
			Offset = val.as<Math::vec2>();
		}
		val = node["PlayOnStart"];
		if (val)
		{
			PlayOnStart = val.as<bool>();
		}
		val = node["SpriteID"];
		if (val)
		{
			SpriteID = GUID::Parse(val.as<std::string>());
			AssetManager::Load(SpriteID);
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
		val = node["Origin"];
		if (val)
		{
			Origin = (SpriteOrigin)Math::clamp(0, (int)SpriteOrigin::BottomRight, val.as<int>());
		}
		val = node["CustomOrigin"];
		if (val)
		{
			Origin = SpriteOrigin::Custom;
			OriginValue = val.as<Math::vec2>();
		}
		val = node["SortingPoint"];
		if (val)
		{
			SortingPoint = val.as<float>();
		}
		else
		{
			if (AssetManager::IsLoaded(SpriteID))
				SortingPoint = AssetManager::Get<Sprite>(SpriteID)->GetSize().y;

		}

	}

	/// @brief Set the sprite to render.
	/// 
	/// Sprite must already be loaded.
	/// @see AssetManager to load sprite.
	/// @param spriteID Name/Path of the sprite.
	void SpriteRenderer::SetSprite(const GUID& spriteID)
	{
		if (AssetManager::IsLoaded(spriteID))
		{
			SpriteID = spriteID;
			auto sprite = AssetManager::Get<Sprite>(spriteID);
			//CHROMA_CORE_INFO("SET SPRITE: {}", spriteID);
			Animation = 0;
			CurrentFrame = 0;
			//SetSpriteOrigin(SpriteOrigin::Default);
			//SortingPoint = sprite->GetSize().y;
		}
			
		else
			CHROMA_CORE_WARN("Attempted to set sprite. Sprite [{}] could not be found or is not loaded.", spriteID.ToString());
	}

	void SpriteRenderer::SetAnimation(unsigned int animation)
	{
		if (AssetManager::IsLoaded(SpriteID))
		{
			auto sprite = AssetManager::Get<Sprite>(SpriteID);
			if (animation >= sprite->Animations.size())
			{
				CHROMA_CORE_WARN("Attempted to set sprite animation number. Sprite [{}] does not contain animation [{}].", sprite->GetPath(), animation);
				Animation = sprite->Animations.size() - 1;
				return;
			}
			bool restart = Animation != animation;
			Animation = animation;
			if (restart)
				RestartAnimation();
			
		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite animation. Sprite [{}] could not be found or is not loaded.", SpriteID.ToString());
		}
			
	}

	void SpriteRenderer::SetAnimation(const std::string& animation_name)
	{

		if (AssetManager::IsLoaded(SpriteID))
		{
			auto sprite = AssetManager::Get<Sprite>(SpriteID);
			bool exists = false;
			unsigned int i = 0;
			for (auto& anim : sprite->Animations)
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
				CHROMA_CORE_WARN("Attempted to set sprite animation. Sprite [{}] does not contain animation [{}]", SpriteID.ToString(), animation_name);
			}
		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite animation. Sprite [{}] could not be found or is not loaded.", SpriteID.ToString());
		}
	}

	unsigned int SpriteRenderer::GetAnimation()
	{
		return Animation;
	}

	std::string SpriteRenderer::GetAnimationName(unsigned int animation)
	{
		if (AssetManager::IsLoaded(SpriteID))
		{
			auto sprite = AssetManager::Get<Sprite>(SpriteID);
			if (animation >= sprite->Animations.size())
				CHROMA_CORE_WARN("Attempted to get sprite animation name. Sprite [{}] does not contain animation [{}]", SpriteID.ToString(), animation);
			else
				return sprite->Animations[animation].Tag;
			
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

		if (AssetManager::IsLoaded(SpriteID))
		{
			auto sprite = AssetManager::Get<Sprite>(SpriteID);
			if (frame >= sprite->Frames.size())
			{
				frame = sprite->Frames.size() - 1;
				CHROMA_CORE_WARN("Attempted to set sprite frame. Frame [{}] is an invalid frame for Sprite [{}].", frame, sprite->GetPath());
			}
			else
				CurrentFrame = frame;

		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite frame. Sprite [{}] could not be found or is not loaded.", SpriteID);
		}

		
	}


	void SpriteRenderer::SetSpriteOrigin(SpriteOrigin origin)
	{
		Origin = origin;
		if (AssetManager::IsLoaded(SpriteID))
		{
			auto sprite = AssetManager::Get<Sprite>(SpriteID);
			const Math::vec2 size = sprite->GetSize();
			switch (origin)
			{
			case SpriteOrigin::Center:
			{
				OriginValue = { Math::floor(size.x / 2), Math::floor(size.y / 2) };
				break;
			}
			case SpriteOrigin::Left:
			{
				OriginValue = { 0, Math::floor(size.y / 2) };
				break;
			}
			case SpriteOrigin::Right:
			{
				OriginValue = { size.x, Math::floor(size.y / 2) };
				break;
			}
			case SpriteOrigin::Top:
			{
				OriginValue = { Math::floor(size.x / 2), 0 };
				break;
			}
			case SpriteOrigin::Bottom:
			{
				OriginValue = { Math::floor(size.x / 2), size.y };
				break;
			}
			case SpriteOrigin::TopLeft:
			{
				OriginValue = { 0,0 };
				break;
			}
			case SpriteOrigin::TopRight:
			{
				OriginValue = { size.x,0 };
				break;
			}
			case SpriteOrigin::BottomLeft:
			{
				OriginValue = { 0,size.y };
				break;
			}
			case SpriteOrigin::BottomRight:
			{
				OriginValue = { size.x,size.y };
				break;
			}
			case SpriteOrigin::Custom:
			{
				CHROMA_CORE_WARN("Attempted to set sprite [{}] origin. Origin was set to 'custom' but no origin was provided.", sprite->GetPath());
				break;
			}
			default:
				OriginValue = { Math::floor(size.x / 2), Math::floor(size.y / 2) };
				Origin = SpriteOrigin::Center;
				break;
			}
			
		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite origin. Sprite [{}] could not be found or is not loaded.", SpriteID.ToString());
		}
	}

	void SpriteRenderer::SetSpriteOrigin(const Math::vec2& custom_position)
	{

		Origin = SpriteOrigin::Custom;
		if (AssetManager::IsLoaded(SpriteID))
		{
			auto sprite = AssetManager::Get<Sprite>(SpriteID);
			const Math::vec2 size = sprite->GetSize();
			OriginValue = { Math::clamp(0.f, size.x, custom_position.x), Math::clamp(0.f, size.y, custom_position.y) };
		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite origin. Sprite [{}] could not be found or is not loaded.", SpriteID.ToString());
		}
	}

	SpriteRenderer::SpriteOrigin SpriteRenderer::GetSpriteOrigin()
	{
		return Origin;
	}

	const Math::vec2& SpriteRenderer::GetSpriteOriginVector()
	{
		return OriginValue;
	}

	std::string SpriteRenderer::GetSpritePath()
	{
		return AssetManager::GetPath(SpriteID);
	}

	void SpriteRenderer::RestartAnimation()
	{

		if (AssetManager::Exists(SpriteID))
		{
			auto sprite = AssetManager::Get<Sprite>(SpriteID);
			if (sprite->Animated())
			{
				if (Animation >= sprite->Animations.size())
					Animation = sprite->Animations.size() - 1;
				else if (Animation < 0)
					Animation = 0;
				CurrentFrame = sprite->Animations[Animation].Start;
			}

		}
	}


	void SpriteRenderer::CreateReflectionModel()
	{	
		entt::meta<SpriteRenderer>()
			.data<&SpriteRenderer::Color>("Color"_hs)
			.data<&SpriteRenderer::Offset>("Offset"_hs)
			.data<&SpriteRenderer::SortingPoint>("SortingPoint"_hs)
			.data<&SpriteRenderer::PlayOnStart>("PlayOnStart"_hs)
			.data<&SpriteRenderer::Playing>("Playing"_hs)
			.data<&SpriteRenderer::Loop>("Loop"_hs)
			.data<&SpriteRenderer::SpeedMultiplier>("SpeedMultiplier"_hs)
			.data<&SpriteRenderer::SetSprite, &SpriteRenderer::GetSpriteID>("SpriteID"_hs)
			//.data<static_cast<void (SpriteRenderer::*)(unsigned int)>(&SpriteRenderer::SetAnimation), &SpriteRenderer::GetAnimation>("Animation"_hs)
			//.data<static_cast<void (SpriteRenderer::*)(const std::string&)>(&SpriteRenderer::SetAnimation), &SpriteRenderer::GetAnimationName>("AnimationName"_hs)
			.data<&SpriteRenderer::SetCurrentFrame, &SpriteRenderer::GetCurrentFrame>("CurrentFrame"_hs)
			//.data<static_cast<void (SpriteRenderer::*)(SpriteOrigin)>(&SpriteRenderer::SetSpriteOrigin), &SpriteRenderer::GetSpriteOrigin>("Origin"_hs)
			//.data<static_cast<void (SpriteRenderer::*)(const Math::vec2&)>(&SpriteRenderer::SetSpriteOrigin), &SpriteRenderer::GetSpriteOriginVector>("OriginVector"_hs)
			.data<nullptr, &SpriteRenderer::GetSpritePath>("SpritePath"_hs)
			.func<&SpriteRenderer::RestartAnimation>("RestartAnimation"_hs)
			.type("SpriteRenderer"_hs);
	}

}


