#include "chromapch.h"
#include "AnimationPlayer.h"

#include "Chroma/Core/Time.h"
#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{

	void AnimationPlayer::Serialize(YAML::Emitter& out)
	{
	}

	void AnimationPlayer::Deserialize(YAML::Node& node)
	{
	}

	void AnimationPlayer::Start()
	{
		m_Time = 0.f;
		m_Keyframes.clear();
		for (auto& track : m_Current->tracks)
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

	void AnimationPlayer::SetAnimation(Animation* animation)
	{
		m_Time = 0.f;
		m_Current = animation;
	}



	void AnimationPlayer::CreateReflectionModel()
	{
		Reflection::RegisterComponent<AnimationPlayer>();
	}
}


