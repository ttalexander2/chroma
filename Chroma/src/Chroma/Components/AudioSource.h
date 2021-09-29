#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Audio/Audio.h"


namespace Chroma
{
	/// @brief Audio Source Component.
	///
	/// This component plays an audio event. Spacial controlling is not yet supported.
	struct AudioSource : Component
	{
		/// @brief Path/Name of FMOD event.
		std::string Event;
		/// @brief GUID of event.
		FMOD_GUID GUID;
		/// @brief Whether the event is currently muted.
		bool Mute = false;
		/// @brief Whether to play the event once the Scene is initialized.
		bool PlayOnInit = false;

		/// @brief Constructs an empty audio source.
		AudioSource() = default;
		/// @brief Constructs a new audio source from an existing audio source.
		/// @param  AudioSource to copy.
		AudioSource(const AudioSource&) = default;
		/// @brief Constructs a new audio source.
		/// @param evnt Name/Path of the FMOD event
		/// @param loop Whether the event should loop.
		/// @param mute Whether the event should start muted.
		/// @param play_on_init Whether the event should play once the Scene is initialized.
		AudioSource(const std::string& evnt, bool loop = false, bool mute = false, bool play_on_init = false)
			: Event(evnt), Mute(mute), PlayOnInit(play_on_init), GUID()
		{
		}

		/// @brief Gets the Component Name.
		/// @return Name of the Component
		const std::string Name() const override
		{
			return StaticName();
		}

		const static std::string StaticName()
		{
			return "Audio Source";
		}

		void Serialize(YAML::Emitter& out) override;

		void Deserialize(YAML::Node& node) override;

	};
}

