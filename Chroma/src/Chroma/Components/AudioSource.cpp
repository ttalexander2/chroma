#include "chromapch.h"
#include "AudioSource.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	Reflection::TypeFactory<AudioSource> AudioSource::register_type()
	{
		return Reflection::register_type<AudioSource>("AudioSource")
		       .base<Component>()
		       .data<&AudioSource::GetEvent, &AudioSource::SetEvent>("Event")
		       .data<&AudioSource::Mute>("Mute")
		       .data<&AudioSource::PlayOnInit>("PlayOnInit");
	}
} //namespace Chroma
