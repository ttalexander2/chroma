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
}