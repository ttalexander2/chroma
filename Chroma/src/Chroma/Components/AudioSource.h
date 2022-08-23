#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Audio/Audio.h"


namespace Chroma
{
	/// @brief Audio Source Component.
	///
	/// This component plays an audio event. Spacial controlling is not yet supported.
	struct AudioSource : public Component
	{
		CHROMA_COMPONENT(AudioSource, Component);
		/// @brief Whether the event is currently muted.
		bool Mute = false;
		/// @brief Whether to play the event once the Scene is initialized.
		bool PlayOnInit = false;

		/// @brief Path/Name of FMOD event.
		std::string Event;
		/// @brief GUID of event.
		FMOD_GUID GUID;

		inline const std::string& GetEvent() const { return Event; }
		inline void SetEvent(const std::string& _event) { Event = _event; GUID = Chroma::Audio::GetEventGuid(Event);}

	};
}

