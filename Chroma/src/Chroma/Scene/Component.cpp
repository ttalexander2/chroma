#include "chromapch.h"
#include "Component.h"
#include "Chroma/Scene/ECS.h"

namespace Chroma
{
	unsigned int Component::component_counter = 1;

	/// @brief Function to begin the serialization process for the component.
	/// 
	/// This function begins the key/map structure for the serialization of data members.
	/// @param out YAML emitter to write to
	void Component::BeginSerialize(YAML::Emitter& out)
	{
		out << YAML::Key << Name();
		out << YAML::Value << YAML::BeginMap;
	}

	/// @brief Function to end the serialization process for the component.
	/// 
	/// This function ends the key/map structure for the serialization of data members.
	/// @param out YAML emitter to write to
	void Component::EndSerialize(YAML::Emitter& out)
	{
		out << YAML::EndMap;
	}

	void Component::DoSerialize(YAML::Emitter& out)
	{
		BeginSerialize(out);
		Serialize(out);
		EndSerialize(out);
	}

	
}