#include "chromapch.h"
#include "AudioSource.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	Reflection::type_factory<AudioSource> AudioSource::register_type()
	{
		return Reflection::RegisterComponent<AudioSource>("AudioSource")
		       .base<Component>()
		       .data<&AudioSource::GetEvent, &AudioSource::SetEvent>("Event")
		       .data<&AudioSource::Mute>("Mute")
		       .data<&AudioSource::PlayOnInit>("PlayOnInit");
	}
} //namespace Chroma
