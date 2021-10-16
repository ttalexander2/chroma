#include "chromapch.h"
#include "BoxCollider.h"
namespace Chroma
{

	void BoxCollider::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Bounds";
		out << YAML::Value << Bounds;

		out << YAML::Key << "Offset";
		out << YAML::Value << Offset;
	}
	void BoxCollider::Deserialize(YAML::Node& node)
	{

		auto val = node["Bounds"];
		if (val)
		{
			Bounds = val.as<Math::vec2>();
		}

		val = node["Offset"];
		if (val)
		{
			Offset = val.as<Math::vec2>();
		}

	}
}