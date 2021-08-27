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
			auto& spriteRenderer = view.get<SpriteRenderer>(e);
			if (!spriteRenderer.PlayOnStart)
				spriteRenderer.Playing = false;
			spriteRenderer.RestartAnimation();
		}
	}

	void SpriteRendererSystem::LateUpdate(Time delta)
	{
		auto view = m_Scene->Registry.view<SpriteRenderer>();
		for (EntityID e : view)
		{
			auto& spriteRenderer = view.get<SpriteRenderer>(e);

			if (AssetManager::HasSprite(spriteRenderer.SpriteID))
				{
					Ref<Sprite> s = AssetManager::GetSprite(spriteRenderer.SpriteID);
					if (spriteRenderer.Playing && s->Animated())
					{
						
						spriteRenderer.SetAnimation(spriteRenderer.Animation);

						Sprite::Animation animation = s->Animations[spriteRenderer.Animation];
						if (spriteRenderer.time_till_next_frame >= s->Frames[spriteRenderer.CurrentFrame].Durration)
						{
							spriteRenderer.time_till_next_frame = 0;
							int start = s->Animations[spriteRenderer.Animation].Start;
							int end = s->Animations[spriteRenderer.Animation].End;

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
						spriteRenderer.time_till_next_frame += delta.GetMilliseconds();

					}
				}

		}
	}

	void SpriteRendererSystem::Draw(Time delta)
	{
		std::map<float, EntityID> sprites;
		for (EntityID e : m_Scene->Registry.view<SpriteRenderer>())
		{
			Transform t = m_Scene->Registry.get<Transform>(e);
			SpriteRenderer s = m_Scene->Registry.get<SpriteRenderer>(e);
			if (!AssetManager::HasSprite(s.SpriteID))
				continue;
			float y = -1.f*(t.Position.y + s.Offset.y - AssetManager::GetSprite(s.SpriteID)->GetSize().y);
			sprites[y] = e;
		}

		//TODO: Cull this shit
			
		for (auto &[y_index, e] : sprites)
		{
			Transform& transform = m_Scene->Registry.get<Transform>(e);
			SpriteRenderer& spriteRenderer = m_Scene->Registry.get<SpriteRenderer>(e);
			Relationship& relationship = m_Scene->Registry.get<Relationship>(e);
			if (AssetManager::HasSprite(spriteRenderer.SpriteID))
			{
				Ref<Sprite> s = AssetManager::GetSprite(spriteRenderer.SpriteID);
				int w = s->Frames[spriteRenderer.CurrentFrame].Texture->GetWidth();
				int h = s->Frames[spriteRenderer.CurrentFrame].Texture->GetHeight();
				if (!relationship.IsChild())
				{
					Chroma::Renderer2D::DrawQuad(transform.Position + spriteRenderer.Offset, transform.Scale * Math::vec3((float)w, (float)h, 1.0f), s->Frames[spriteRenderer.CurrentFrame].Texture, spriteRenderer.Color, transform.Rotation.z);
				}
				else
				{
					Math::vec3 pos = transform.Position;
					Math::vec3 scale = transform.Scale;
					Math::vec3 rotation = transform.Rotation;
					EntityID parent = relationship.Parent;
					while (parent != ENTITY_NULL)
					{
						Transform& parentTransform = m_Scene->GetComponent<Transform>(parent);
						pos += parentTransform.Position;
						scale *= parentTransform.Scale;
						rotation += parentTransform.Rotation;
						parent = m_Scene->GetComponent<Relationship>(parent).Parent;
					}
					Chroma::Renderer2D::DrawQuad(pos + spriteRenderer.Offset, scale * Math::vec3((float)w, (float)h, 1.0f), s->Frames[spriteRenderer.CurrentFrame].Texture, spriteRenderer.Color, rotation.z);
				}

			}
		}
	}
}


