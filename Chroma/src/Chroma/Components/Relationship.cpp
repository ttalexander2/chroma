#include "chromapch.h"
#include "Relationship.h"

namespace Chroma
{
	void Relationship::Serialize(YAML::Emitter& out)
	{
		if (Parent != ENTITY_NULL)
		{
			out << YAML::Key << "Parent";
			out << YAML::Value << (uint32_t)Parent;
		}

		if (HasChildren())
		{
			out << YAML::Key << "Children";
			out << YAML::Value << YAML::Flow << YAML::BeginSeq;

			for (EntityID id : Children)
			{
				out << (uint32_t)id;
			}

			out << YAML::EndSeq;
		}

	}

	void Relationship::Deserialize(YAML::Node& node)
	{
		auto val = node["Parent"];
		if (val)
		{
			Parent = (EntityID)val.as<uint32_t>();
		}

		val = node["Children"];
		if (val && val.IsSequence())
		{
			for (auto it = val.begin(); it != val.end(); it++)
			{
				const auto& item = *it;
				Children.push_back((EntityID)item.as<uint32_t>());
			}
		}
	}
}


