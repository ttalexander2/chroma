#include "chromapch.h"
#include "BoxCollider2D.h"
namespace Chroma
{

	void BoxCollider2D::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Bounds";
		out << YAML::Value << Bounds;

		out << YAML::Key << "Offset";
		out << YAML::Value << Offset;
	}
	void BoxCollider2D::Deserialize(YAML::Node& node)
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