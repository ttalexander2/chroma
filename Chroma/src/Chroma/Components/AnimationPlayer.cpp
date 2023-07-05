#include "chromapch.h"
#include "AnimationPlayer.h"

#include "Chroma/Core/Time.h"
#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	void AnimationPlayer::Start()
	{
		m_Time = 0.f;
		m_Keyframes.clear();
		for (auto &track : m_Current->tracks)
		{
			m_Keyframes[&track] = track.keyframes.begin();
		}
		m_Playing = true;
	}

	void AnimationPlayer::Stop()
	{
		m_Playing = false;
		m_Time = 0.f;
	}

	void AnimationPlayer::Restart()
	{
		Start();
	}

	void AnimationPlayer::Pause()
	{
		m_Playing = false;
	}

	void AnimationPlayer::SetAnimation(Animation *animation)
	{
		m_Time = 0.f;
		m_Current = animation;
	}

	Reflection::TypeFactory<AnimationPlayer> AnimationPlayer::register_type()
	{
		return Reflection::register_type<AnimationPlayer>("AnimationPlayer")
		       .base<Component>()
		       .data<&AnimationPlayer::PlayOnStart>("PlayOnStart");
	}
}
