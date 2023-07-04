#pragma once

#include "YamlSerializer.h"

namespace Chroma::Reflection
{
	class Serializer
	{
	public:
		static void SerializeObjectToYaml(YAML::Emitter& out, Handle object);
		static void DeserializeObjectFromYaml(Handle object, YAML::Node& node);
	};
}

