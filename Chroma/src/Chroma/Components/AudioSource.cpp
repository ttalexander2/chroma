#include "chromapch.h"
#include "AudioSource.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{

	Reflection::TypeFactory<AudioSource> AudioSource::RegisterType()
	{
		return Reflection::Register<AudioSource>("AudioSource")
				.Base<Component>()
				.Data<&AudioSource::SetEvent, &AudioSource::GetEvent>("Event")
				.Data<&AudioSource::Mute>("Mute")
				.Data<&AudioSource::PlayOnInit>("PlayOnInit");
	}

} //namespace Chroma