#include "chromapch.h"
#include "SpriteRendererSystem.h"
#include "Chroma/Components/SpriteRenderer.h"
#include "Chroma/Scene/Entity.h"
#include "Chroma/Components/Transform.h"
#include <Chroma/Renderer/Renderer2D.h>
#include <Chroma/Assets/AssetManager.h>
#include <Chroma/ImGui/ImGuiDebugMenu.h>

namespace Chroma
{
	void SpriteRendererSystem::Load()
	{
		/*
		auto view = m_Scene->Registry.view<SpriteRenderer>();

		AssetManager::BeginSpriteLoad();
		for (EntityID e : view)
		{
			auto& renderer = view.get<SpriteRenderer>(e);
			AssetManager::LoadSprite(renderer.SpriteID);
				
		}
		AssetManager::EndSpriteLoad();
		*/
	}

	void SpriteRendererSystem::Init()
	{
		auto view = m_Scene->Registry.view<SpriteRenderer>();
		for (EntityID e : view)
		{
			auto &spriteRenderer = view.get<SpriteRenderer>(e);
			spriteRenderer.Playing = spriteRenderer.PlayOnStart;
			spriteRenderer.RestartAnimation();
		}
	}

	void SpriteRendererSystem::LateUpdate(Time delta)
	{
		auto view = m_Scene->Registry.view<SpriteRenderer>();
		for (EntityID e : view)
		{
			auto &spriteRenderer = view.get<SpriteRenderer>(e);
			if (!spriteRenderer.IsEnabled())
				continue;

			if (spriteRenderer.GetSprite() && spriteRenderer.GetSprite()->IsLoaded())
			{
				auto &sprite = spriteRenderer.GetSprite();
				if (spriteRenderer.Playing && sprite->Animated())
				{
					spriteRenderer.SetAnimation(spriteRenderer.Animation);

					Sprite::Animation animation = sprite->Animations[spriteRenderer.Animation];
					if (spriteRenderer.time_till_next_frame >= sprite->Frames[spriteRenderer.CurrentFrame].Duration)
					{
						spriteRenderer.time_till_next_frame = 0;
						int start = sprite->Animations[spriteRenderer.Animation].Start;
						int end = sprite->Animations[spriteRenderer.Animation].End;

						if (animation.Direction == Sprite::LoopDirection::Forward)
						{
							spriteRenderer.CurrentFrame++;
						}
						else if (animation.Direction == Sprite::LoopDirection::Reverse)
						{
							spriteRenderer.CurrentFrame--;
						}
						else
						{
							if (spriteRenderer.looping_forward)
								spriteRenderer.CurrentFrame++;
							else
								spriteRenderer.CurrentFrame--;
						}

						if ((spriteRenderer.looping_forward && spriteRenderer.CurrentFrame >= animation.End)
							|| (!spriteRenderer.looping_forward && spriteRenderer.CurrentFrame <= animation.Start))
						{
							if (animation.Direction == Sprite::LoopDirection::PingPong)
							{
								spriteRenderer.looping_forward = !spriteRenderer.looping_forward;
								//LAST FRAME
							}
						}
						if (spriteRenderer.Loop)
						{
							int high = animation.End;
							int low = animation.Start;
							int diff = high - low + 1;
							spriteRenderer.CurrentFrame = Math::abs((spriteRenderer.CurrentFrame - low) % diff) + low;
						}
						else
						{
							if (spriteRenderer.CurrentFrame >= animation.End)
							{
								spriteRenderer.Playing = false;
							}
						}
					}
					spriteRenderer.time_till_next_frame += static_cast<float>(delta.GetMilliseconds());
				}
			}
		}
	}

	void SpriteRendererSystem::Draw(Time delta)
	{
		std::map<float, std::vector<EntityID>> sprites;
		for (EntityID e : m_Scene->Registry.view<SpriteRenderer>())
		{
			auto s = m_Scene->Registry.get<SpriteRenderer>(e);
			if (!s.IsEnabled())
				continue;

			Math::vec2 absPos = m_Scene->GetTransformAbsolutePosition(e);
			if (!s.GetSprite() || !s.GetSprite()->IsLoaded())
				continue;
			float y = -1.f * (absPos.y + s.Offset.y + s.SortingPoint);
			if (!sprites.contains(y))
				sprites[y] = std::vector<EntityID>();
			sprites[y].push_back(e);
		}

		//TODO: Cull sprites?

		for (auto &[y_index, sp_list] : sprites)
		{
			for (auto &e : sp_list)
			{
				Transform &transform = m_Scene->Registry.get<Transform>(e);
				SpriteRenderer &spriteRenderer = m_Scene->Registry.get<SpriteRenderer>(e);
				if (!spriteRenderer.IsEnabled())
					continue;

				const Math::vec2 &origin = spriteRenderer.GetSpriteOriginVector();

				if (spriteRenderer.GetSprite()->IsLoaded())
				{
					auto sprite = spriteRenderer.GetSprite();
					int w = sprite->Frames[spriteRenderer.CurrentFrame].Texture->GetWidth();
					int h = sprite->Frames[spriteRenderer.CurrentFrame].Texture->GetHeight();
					if (!transform.IsChild())
					{
						Math::vec2 size = transform.Scale * Math::vec2(static_cast<float>(w), static_cast<float>(h));
						Math::vec2 originAdjustment = { Math::abs(size.x) / 2.f - origin.x, -Math::abs(size.y) / 2.f + origin.y };

						Renderer2D::DrawSprite(static_cast<int>(e), transform.Position + spriteRenderer.Offset + originAdjustment * transform.Scale, size, sprite->Frames[spriteRenderer.CurrentFrame].Texture, spriteRenderer.Color, transform.Rotation);
					}
					else
					{
						Math::vec2 pos = transform.Position;
						Math::vec2 scale = transform.Scale;
						Math::vec2 parentPos{ 0, 0 };
						float parentRot = 0;
						float rotation = transform.Rotation;
						EntityID parent = transform.Parent;
						while (parent != ENTITY_NULL)
						{
							const Transform &parentTransform = m_Scene->GetComponent<Transform>(parent);
							parentPos += parentTransform.Position;
							scale *= parentTransform.Scale;
							parentRot += parentTransform.Rotation;
							parent = parentTransform.Parent;
						}

						Math::vec2 adjusted = { pos.x * Math::cos(parentRot) - pos.y * Math::sin(parentRot), pos.x * Math::sin(parentRot) + pos.y * Math::cos(parentRot) };
						//Math::vec2 finalPos = parentPos + adjusted + spriteRenderer.Offset;
						//CHROMA_CORE_TRACE("Adjusted: [{}, {}]; ParentPos: [{}, {}]; ParentRot: {}, FINAL: [{},{}]", adjusted.x, adjusted.y, parentPos.x, parentPos.y, parentRot, finalPos.x, finalPos.y);

						Math::vec2 size = scale * Math::vec2(static_cast<float>(w), static_cast<float>(h));
						Math::vec2 originAdjustment = { Math::abs(size.x) / 2.f - origin.x, -Math::abs(size.y) / 2.f + origin.y };

						Renderer2D::DrawSprite(static_cast<int>(e), parentPos + adjusted + spriteRenderer.Offset + originAdjustment * transform.Scale, size, sprite->Frames[spriteRenderer.CurrentFrame].Texture, spriteRenderer.Color, rotation + parentRot);
					}
				}
			}
		}
	}
}
