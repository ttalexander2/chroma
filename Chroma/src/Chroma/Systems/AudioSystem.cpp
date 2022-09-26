#include "chromapch.h"
#include "AudioSystem.h"

#include "Chroma/Components/AudioSource.h"
#include "Chroma/Scene/Entity.h"

namespace Chroma
{
	void AudioSystem::LateInit()
	{
		auto view = m_Scene->Registry.view<AudioSource>();
		for (EntityID e : view)
		{
			AudioSource &audio = view.get<AudioSource>(e);
			Audio::LoadEvent(audio.Event);
			if (audio.PlayOnInit && audio.IsEnabled())
				Audio::PlayEvent(audio.Event);
		}
	}


	void AudioSystem::LateUpdate(Time delta)
	{
		auto view = m_Scene->Registry.view<AudioSource>();
		for (EntityID e : view)
		{
			AudioSource &audio = view.get<AudioSource>(e);
			if (!audio.IsEnabled())
				continue;
		}
	}
}
