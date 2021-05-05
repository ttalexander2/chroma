#include "chromapch.h"
#include "Component.h"

namespace Chroma
{
	unsigned int Component::component_counter = 1;

	void Component::BeginSerialize(YAML::Emitter& out)
	{
		out << YAML::Key << Name();
		out << YAML::Value << YAML::BeginMap;
	}
	void Component::EndSerialize(YAML::Emitter& out)
	{
		out << YAML::EndMap;
	}
}