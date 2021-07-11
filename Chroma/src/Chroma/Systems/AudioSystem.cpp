#include "chromapch.h"
#include "AudioSystem.h"

#include "Chroma/Components/AudioSource.h"
#include "Chroma/Scene/EntityRef.h"

namespace Chroma
{
	void AudioSystem::LateInit()
	{
		for (EntityRef e : m_Scene->View<AudioSource>())
		{
			ComponentRef<AudioSource> audio = e.GetComponent<AudioSource>();
			Audio::LoadEvent(audio->Event);
			if (audio->PlayOnInit)
				Audio::PlayEvent(audio->Event);
		}
	}

	
	void AudioSystem::LateUpdate(Time delta)
	{
		for (EntityRef e : m_Scene->View<AudioSource>())
		{
			ComponentRef<AudioSource> audio = e.GetComponent<AudioSource>();
		}
	}
}


