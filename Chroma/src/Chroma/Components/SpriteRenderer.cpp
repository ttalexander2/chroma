#include "chromapch.h"
#include "SpriteRenderer.h"
#include "Chroma/Core/Application.h"
#include "Chroma/Assets/AssetManager.h"

namespace Chroma
{
	
	void SpriteRenderer::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Color";
		out << YAML::Value << Color;

		out << YAML::Key << "Offset";
		out << YAML::Value << Offset;

		out << YAML::Key << "Layer";
		out << YAML::Value << Layer.ToString();

		out << YAML::Key << "PlayOnStart";
		out << YAML::Value << PlayOnStart;

		out << YAML::Key << "SpriteID";
		out << YAML::Value << SpriteID;

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
		val = node["Layer"];
		if (val)
		{
			Layer = GUID::Parse(val.as<std::string>());
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
			if (AssetManager::HasSprite(SpriteID))
			{
				SortingPoint = AssetManager::GetSprite(SpriteID)->GetSize().y;
			}
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
			//CHROMA_CORE_INFO("SET SPRITE: {}", spriteID);
			SpriteID = spriteID;
			Animation = 0;
			CurrentFrame = 0;
			SetSpriteOrigin(SpriteOrigin::Default);
			SortingPoint = AssetManager::GetSprite(SpriteID)->GetSize().y;
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

	unsigned int SpriteRenderer::GetAnimation()
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
			{
				frame = s->Frames.size() - 1;
				CHROMA_CORE_WARN("Attempted to set sprite frame. Frame [{}] is an invalid frame for Sprite [{}].", frame, SpriteID);
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
		if (AssetManager::HasSprite(SpriteID))
		{
			Ref<Sprite> s = AssetManager::GetSprite(SpriteID);
			const Math::vec2 size = s->GetSize();
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
				CHROMA_CORE_WARN("Attempted to set sprite [{}] origin. Origin was set to 'custom' but no origin was provided.", SpriteID);
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
			CHROMA_CORE_WARN("Attempted to set sprite origin. Sprite [{}] could not be found or is not loaded.", SpriteID);
		}
	}

	void SpriteRenderer::SetSpriteOrigin(const Math::vec2& custom_position)
	{
		Origin = SpriteOrigin::Custom;
		if (AssetManager::HasSprite(SpriteID))
		{
			Ref<Sprite> s = AssetManager::GetSprite(SpriteID);
			const Math::vec2 size = s->GetSize();
			OriginValue = { Math::clamp(0.f, size.x, custom_position.x), Math::clamp(0.f, size.y, custom_position.y) };
		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite origin. Sprite [{}] could not be found or is not loaded.", SpriteID);
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


