#include "chromapch.h"
#include "AudioSource.h"


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
		entt::meta<AudioSource>()
			.data<&AudioSource::SetEvent, &AudioSource::GetEvent>("Event"_hs)
			.data<&AudioSource::Mute>("Mute"_hs)
			.data<&AudioSource::PlayOnInit>("PlayOnInit"_hs)
			.type("AudioSource"_hs);
	}
}