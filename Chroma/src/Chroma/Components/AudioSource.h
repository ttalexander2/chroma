#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Audio/Audio.h"


namespace Chroma
{
	struct AudioSource : Component
	{
		std::string Event;
		FMOD_GUID GUID;
		bool Mute = false;
		bool PlayOnInit = false;

		AudioSource() = default;
		AudioSource(const AudioSource&) = default;
		AudioSource(const std::string& evnt, bool loop = false, bool mute = false, bool play_on_init = false)
			: Event(evnt), Mute(mute), PlayOnInit(play_on_init), GUID()
		{
		}

		const std::string Name() const override
		{
			return "Audio Source";
		}

		void DrawImGui() override;

		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;

	};
}

