#include "chromapch.h"
#include "AnimationSystem.h"

#include "Chroma/Components/AnimationPlayer.h"
#include "Chroma/Reflection/Reflection.h"

#include <glm/gtx/spline.hpp>

namespace Chroma
{

	void AnimationSystem::Init()
	{
		for (Chroma::EntityID entity : m_Scene->Registry.view<Chroma::AnimationPlayer>())
		{
			Chroma::AnimationPlayer& aPlayer = m_Scene->GetComponent<Chroma::AnimationPlayer>(entity);
			aPlayer.m_Time = 0.f;
			if (aPlayer.PlayOnStart)
				aPlayer.Start();

		}
	}

	void AnimationSystem::Update(Time delta)
	{
		for (Chroma::EntityID entity : m_Scene->Registry.view<Chroma::AnimationPlayer>())
		{
			Chroma::AnimationPlayer& aPlayer = m_Scene->GetComponent<Chroma::AnimationPlayer>(entity);

			if (aPlayer.m_Playing)
			{
				aPlayer.m_Time += delta.GetSeconds();

				if (aPlayer.m_Time >= aPlayer.m_Current->length)
				{
					if (aPlayer.m_Current->loop_type == Chroma::Animation::LoopType::loop)
						aPlayer.Restart();
					else if (aPlayer.m_Current->loop_type == Chroma::Animation::LoopType::pingPong)
						aPlayer.m_Reverse = !aPlayer.m_Reverse;
					else
						aPlayer.Stop();
				}

				for (auto& [track, kf] : aPlayer.m_Keyframes)
				{
					std::multiset<Chroma::Animation::Keyframe, Chroma::Animation::TimeSort>::iterator next;


					if (kf->time >= aPlayer.m_Current->length)
					{
						
						if (next == track->keyframes.end())
						{
							if (aPlayer.m_Current->loop_type == Chroma::Animation::LoopType::loop)
							{
								aPlayer.m_Keyframes[track] = track->keyframes.begin();
							}
							else if (aPlayer.m_Current->loop_type == Chroma::Animation::LoopType::pingPong)
							{
								aPlayer.m_Keyframes[track] = track->keyframes.begin();
							}
						}
						aPlayer.m_Keyframes[track] = std::next(kf, 1);
					}
				}

				for (auto& [track, kf] : aPlayer.m_Keyframes)
				{
					std::multiset<Chroma::Animation::Keyframe, Chroma::Animation::TimeSort>::iterator next;

					if (aPlayer.m_Reverse)
						next = std::next(kf, -1);
					else
						next = std::next(kf, 1);

					if (next == track->keyframes.end())
					{
						continue;
					}
						

					if (track->update == Chroma::Animation::UpdateType::continuous)
					{

						if (kf->value.IsType<Math::vec2>())
						{

							Math::vec2 a = *kf->value.TryCast<Math::vec2>();
							Math::vec2 b = *next->value.TryCast<Math::vec2>();

							Chroma::Animation::Transition t = kf->transition;

							float transitionTime = (next->time - kf->time);
							float timeSinceStart = delta.GetSeconds() - kf->time;
							Math::vec2 val = glm::cubic(a, t.a, t.b, b, timeSinceStart / transitionTime);

							Chroma::Component *c = m_Scene->GetComponent(track->component, track->entityID);
							if (c != nullptr)
							{
								auto meta_handle = c->GetType().Data(track->property);
								meta_handle.Set(Reflection::Handle(c), val);
							}

						}
					}
					else
					{
						if (next->time >= aPlayer.m_Time)
						{
							Chroma::Component* c = m_Scene->GetComponent(track->component, track->entityID);
							if (c != nullptr)
							{
								auto meta_handle = c->GetType().Data(track->property);
								meta_handle.Set(Reflection::Handle(c), kf->value);
							}
						}
					}
				}
					

				
			}


		}
	}

}


