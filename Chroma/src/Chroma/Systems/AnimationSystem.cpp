#include "chromapch.h"
#include "AnimationSystem.h"

#include "Chroma/Components/AnimationPlayer.h"
#include "Chroma/Reflection/Reflection.h"

#include <glm/gtx/spline.hpp>

namespace Chroma
{
	void AnimationSystem::Init()
	{
		for (EntityID entity : m_Scene->Registry.view<AnimationPlayer>())
		{
			AnimationPlayer &aPlayer = m_Scene->GetComponent<AnimationPlayer>(entity);
			aPlayer.m_Time = 0.f;
			if (aPlayer.PlayOnStart)
				aPlayer.Start();
		}
	}

	void AnimationSystem::Update(Time delta)
	{
		for (EntityID entity : m_Scene->Registry.view<AnimationPlayer>())
		{
			AnimationPlayer &aPlayer = m_Scene->GetComponent<AnimationPlayer>(entity);

			if (aPlayer.m_Playing)
			{
				aPlayer.m_Time += static_cast<float>(delta.GetSeconds());

				if (aPlayer.m_Time >= aPlayer.m_Current->length)
				{
					if (aPlayer.m_Current->loop_type == Animation::LoopType::loop)
						aPlayer.Restart();
					else if (aPlayer.m_Current->loop_type == Animation::LoopType::pingPong)
						aPlayer.m_Reverse = !aPlayer.m_Reverse;
					else
						aPlayer.Stop();
				}

				for (auto &[track, kf] : aPlayer.m_Keyframes)
				{
					std::multiset<Animation::Keyframe, Animation::TimeSort>::iterator next;

					if (kf->time >= aPlayer.m_Current->length)
					{
						if (next == track->keyframes.end())
						{
							if (aPlayer.m_Current->loop_type == Animation::LoopType::loop)
							{
								aPlayer.m_Keyframes[track] = track->keyframes.begin();
							}
							else if (aPlayer.m_Current->loop_type == Animation::LoopType::pingPong)
							{
								aPlayer.m_Keyframes[track] = track->keyframes.begin();
							}
						}
						aPlayer.m_Keyframes[track] = std::next(kf, 1);
					}
				}

				for (auto &[track, kf] : aPlayer.m_Keyframes)
				{
					std::multiset<Animation::Keyframe, Animation::TimeSort>::iterator next;

					if (aPlayer.m_Reverse)
						next = std::next(kf, -1);
					else
						next = std::next(kf, 1);

					if (next == track->keyframes.end())
					{
						continue;
					}

					if (track->update == Animation::UpdateType::continuous)
					{
						if (kf->value.IsType<Math::vec2>())
						{
							Math::vec2 a = *kf->value.TryCast<Math::vec2>();
							Math::vec2 b = *next->value.TryCast<Math::vec2>();

							Animation::Transition t = kf->transition;

							float transitionTime = (next->time - kf->time);
							float timeSinceStart = static_cast<float>(delta.GetSeconds()) - kf->time;
							Math::vec2 val = cubic(a, t.a, t.b, b, timeSinceStart / transitionTime);

							Component *c = m_Scene->GetComponent(track->componentID, track->entityID);

							auto meta_handle = c->GetType().Data(track->propertyID);
							Reflection::Handle cHandle = c;
							meta_handle.Set(cHandle, val);
						}
					}
					else
					{
						if (next->time >= aPlayer.m_Time)
						{
							Component *c = m_Scene->GetComponent(track->componentID, track->entityID);

							auto meta_handle = c->GetType().Data(track->propertyID);
							Reflection::Handle cHandle = c;
							meta_handle.Set(cHandle, kf->value);
						}
					}
				}
			}
		}
	}
}
