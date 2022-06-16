#include "chromapch.h"
#include "CircleCollider.h"

#include "Chroma/Reflection/Reflection.h"

namespace Chroma
{
	void CircleCollider::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Radius";
		out << YAML::Value << Radius;

		out << YAML::Key << "Offset";
		out << YAML::Value << Offset;
	}

	void CircleCollider::Deserialize(YAML::Node& node)
	{
		auto val = node["Radius"];
		if (val)
		{
			Radius = val.as<float>();
		}

		val = node["Offset"];
		if (val)
		{
			Offset = val.as<Math::vec2>();
		}

	}

	void CircleCollider::CreateReflectionModel()
	{
		Reflection::RegisterComponent<CircleCollider>();
		Reflection::RegisterComponentProperty<CircleCollider, &CircleCollider::Radius>("Radius");
		Reflection::RegisterComponentProperty<CircleCollider, &CircleCollider::Offset>("Offset");
	}
}