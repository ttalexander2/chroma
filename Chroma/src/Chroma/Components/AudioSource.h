#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Audio/Audio.h"

namespace Chroma
{
	struct AudioSource : Component
	{
		std::string Event;
		float Volume = 1.0f;
		bool Mute = false;
		bool Loop = false;
		bool PlayOnInit = false;

		AudioSource() = default;
		AudioSource(const AudioSource&) = default;
		AudioSource(const std::string& evnt, float volume = 1.0f, bool loop = false, bool mute = false, bool play_on_init = false)
			: Event(evnt), Loop(loop), Volume(volume), Mute(mute), PlayOnInit(play_on_init)
		{
		}

		const std::string Name() const override
		{
			return "Audio Source";
		}

	};
}

