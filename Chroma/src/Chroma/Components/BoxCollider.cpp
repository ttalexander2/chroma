#include "chromapch.h"
#include "BoxCollider.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{

	void BoxCollider::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Min";
		out << YAML::Value << Min;

		out << YAML::Key << "Max";
		out << YAML::Value << Max;
	}


	void BoxCollider::Deserialize(YAML::Node& node)
	{

		auto val = node["Min"];
		if (val)
		{
			Min = val.as<Math::vec2>();
		}

		val = node["Max"];
		if (val)
		{
			Max = val.as<Math::vec2>();
		}

	}

	void BoxCollider::CreateReflectionModel()
	{
		Reflection::RegisterComponent<BoxCollider>();
		Reflection::RegisterComponentProperty<BoxCollider, &BoxCollider::Min>("Min");
		Reflection::RegisterComponentProperty<BoxCollider, &BoxCollider::Max>("Max");
	}
}