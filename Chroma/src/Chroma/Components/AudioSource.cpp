#include "chromapch.h"
#include "AudioSource.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{

	void AudioSource::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Event";
		out << YAML::Value << Event;
	}
	void AudioSource::Deserialize(YAML::Node& node)
	{
		auto val = node["Event"];
		if (val)
		{
			Event = val.as<std::string>();
		}

	}
	void AudioSource::CreateReflectionModel()
	{
		Reflection::RegisterComponent<AudioSource, Component>();
		Reflection::RegisterComponentProperty<AudioSource, &AudioSource::SetEvent, &AudioSource::GetEvent>("Event");
		Reflection::RegisterComponentProperty<AudioSource, &AudioSource::Mute>("Mute");
		Reflection::RegisterComponentProperty<AudioSource, &AudioSource::PlayOnInit>("PlayOnInit");
	}
}