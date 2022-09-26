#include "chromapch.h"
#include "SpriteRenderer.h"
#include "Chroma/Core/Application.h"
#include "Chroma/Assets/AssetManager.h"

#include <functional>

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	/// @brief Set the sprite to render.
	/// 
	/// Sprite must already be loaded.
	/// @see AssetManager to load sprite.
	/// @param spriteID Name/Path of the sprite.
	void SpriteRenderer::SetSprite(const GUID &spriteID)
	{
		m_SpriteID = spriteID;
		if (AssetManager::Exists(spriteID) && !AssetManager::IsLoaded(spriteID))
		{
			AssetManager::Load(spriteID);
		}
		if (AssetManager::Exists(spriteID) && AssetManager::IsLoaded(spriteID))
		{
			Animation = 0;
			CurrentFrame = 0;
		}
	}

	void SpriteRenderer::SetAnimation(unsigned int animation)
	{
		auto sprite = AssetManager::Get<Sprite>(m_SpriteID);
		if (sprite && sprite->IsLoaded())
		{
			if (animation >= sprite->Animations.size())
			{
				CHROMA_CORE_WARN("Attempted to set sprite animation number. Sprite [{}] does not contain animation [{}].", sprite->GetPath(), animation);
				Animation = static_cast<unsigned int>(sprite->Animations.size()) - 1;
				return;
			}
			bool restart = Animation != animation;
			Animation = animation;
			if (restart)
				RestartAnimation();
		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite animation. Sprite could not be found or is not loaded.");
		}
	}

	void SpriteRenderer::SetAnimation(const std::string &animation_name)
	{
		auto sprite = AssetManager::Get<Sprite>(m_SpriteID);
		if (sprite && sprite->IsLoaded())
		{
			bool exists = false;
			unsigned int i = 0;
			for (auto &anim : sprite->Animations)
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
				CHROMA_CORE_WARN("Attempted to set sprite animation. Sprite [{}] does not contain animation [{}]", sprite->GetID().ToString(), animation_name);
			}
		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite animation. Sprite could not be found or is not loaded.");
		}
	}

	unsigned int SpriteRenderer::GetAnimation()
	{
		return Animation;
	}

	std::string SpriteRenderer::GetAnimationName(unsigned int animation)
	{
		auto sprite = AssetManager::Get<Sprite>(m_SpriteID);
		if (sprite && sprite->IsLoaded())
		{
			if (animation >= sprite->Animations.size())
				CHROMA_CORE_WARN("Attempted to get sprite animation name. Sprite [{}] does not contain animation [{}]", sprite->GetID().ToString(), animation);
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

		auto sprite = AssetManager::Get<Sprite>(m_SpriteID);

		if (sprite && sprite->IsLoaded())
		{
			if (frame >= sprite->Frames.size())
			{
				frame = static_cast<unsigned int>(sprite->Frames.size()) - 1;
				CHROMA_CORE_WARN("Attempted to set sprite frame. Frame [{}] is an invalid frame for Sprite [{}].", frame, sprite->GetPath());
			}
			else
				CurrentFrame = frame;
		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite frame. Sprite could not be found or is not loaded.");
		}
	}


	void SpriteRenderer::SetSpriteOrigin(SpriteOrigin origin)
	{
		auto sprite = AssetManager::Get<Sprite>(m_SpriteID);

		Origin = origin;
		if (sprite && sprite->IsLoaded())
		{
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
					OriginValue = { 0, 0 };
					break;
				}
				case SpriteOrigin::TopRight:
				{
					OriginValue = { size.x, 0 };
					break;
				}
				case SpriteOrigin::BottomLeft:
				{
					OriginValue = { 0, size.y };
					break;
				}
				case SpriteOrigin::BottomRight:
				{
					OriginValue = { size.x, size.y };
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
			CHROMA_CORE_WARN("Attempted to set sprite origin. Sprite could not be found or is not loaded.");
		}
	}

	void SpriteRenderer::SetSpriteOrigin(const Math::vec2 &custom_position)
	{
		auto sprite = AssetManager::Get<Sprite>(m_SpriteID);

		Origin = SpriteOrigin::Custom;
		if (sprite && sprite->IsLoaded())
		{
			const Math::vec2 size = sprite->GetSize();
			OriginValue = { Math::clamp(0.f, size.x, custom_position.x), Math::clamp(0.f, size.y, custom_position.y) };
		}
		else
		{
			CHROMA_CORE_WARN("Attempted to set sprite origin. Sprite could not be found or is not loaded.");
		}
	}

	SpriteRenderer::SpriteOrigin SpriteRenderer::GetSpriteOrigin()
	{
		return Origin;
	}

	const Math::vec2 &SpriteRenderer::GetSpriteOriginVector()
	{
		return OriginValue;
	}

	std::string SpriteRenderer::GetSpritePath()
	{
		auto sprite = AssetManager::Get<Sprite>(m_SpriteID);
		if (!sprite)
			return "";
		return sprite->GetPath();
	}

	void SpriteRenderer::RestartAnimation()
	{
		auto sprite = AssetManager::Get<Sprite>(m_SpriteID);
		if (sprite && sprite->IsLoaded())
		{
			if (sprite->Animated())
			{
				if (Animation >= sprite->Animations.size())
					Animation = static_cast<unsigned int>(sprite->Animations.size()) - 1;
				else if (Animation < 0)
					Animation = 0;
				CurrentFrame = sprite->Animations[Animation].Start;
			}
		}
	}

	Reflection::TypeFactory<SpriteRenderer> SpriteRenderer::RegisterType()
	{
		Reflection::Register<SpriteOrigin>("SpriteOrigin")
				.Data<SpriteOrigin::Center>("Center")
				.Data<SpriteOrigin::Left>("Left")
				.Data<SpriteOrigin::Right>("Right")
				.Data<SpriteOrigin::Top>("Top")
				.Data<SpriteOrigin::Bottom>("Bottom")
				.Data<SpriteOrigin::TopLeft>("TopLeft")
				.Data<SpriteOrigin::TopRight>("TopRight")
				.Data<SpriteOrigin::BottomLeft>("BottomLeft")
				.Data<SpriteOrigin::BottomRight>("BottomRight")
				.Data<SpriteOrigin::Custom>("Custom")
				.Data<SpriteOrigin::Default>("Default");

		return Reflection::Register<SpriteRenderer>("SpriteRenderer")
		       .Base<Component>()
		       .Data<&SpriteRenderer::Color>("Color")
		       .Data<&SpriteRenderer::Offset>("Offset")
		       .Data<&SpriteRenderer::SortingPoint>("SortingPoint")
		       .Data<&SpriteRenderer::PlayOnStart>("PlayOnStart")
		       .Data<&SpriteRenderer::Playing>("Playing")
		       .Data<&SpriteRenderer::Loop>("Loop")
		       .Data<&SpriteRenderer::SpeedMultiplier>("SpeedMultiplier")
		       .Data<&SpriteRenderer::SetSprite, &SpriteRenderer::GetSpriteID>("SpriteID")
		       .Data<&SpriteRenderer::SetCurrentFrame, &SpriteRenderer::GetCurrentFrame>("CurrentFrame", false)
		       .Data<static_cast<void (SpriteRenderer::*)(unsigned int)>(&SpriteRenderer::SetAnimation), &SpriteRenderer::GetAnimation>("Animation", false)
		       .Data<static_cast<void (SpriteRenderer::*)(SpriteOrigin)>(&SpriteRenderer::SetSpriteOrigin), &GetSpriteOrigin>("SpriteOrigin")
		       .Func<&SpriteRenderer::GetSpritePath>("GetSpritePath")
		       .Func<&SpriteRenderer::RestartAnimation>("RestartAnimation")
		       .Func<static_cast<void (SpriteRenderer::*)(const std::string &)>(&SpriteRenderer::SetAnimation)>("SetAnimation");
	}
}
