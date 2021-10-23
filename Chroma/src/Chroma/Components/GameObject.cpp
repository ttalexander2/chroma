#include "chromapch.h"
#include "GameObject.h"

namespace Chroma
{
	void GameObject::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Name";
		out << YAML::Value << Name;

		if (Parent != GUID::Zero())
		{
			out << YAML::Key << "Parent";
			out << YAML::Value << Parent.ToString();
		}

		if (HasChildren())
		{
			out << YAML::Key << "Children";
			out << YAML::Value << YAML::Flow << YAML::BeginSeq;

			for (GUID id : Children)
			{
				out << id.ToString();
			}

			out << YAML::EndSeq;
		}
	}

	void GameObject::Deserialize(YAML::Node& node)
	{
		auto val = node["Name"];
		if (val)
		{
			Name = val.as<std::string>();
		}

		val = node["Parent"];
		if (val)
		{
			Parent = GUID::Parse(val.as<std::string>());
		}

		val = node["Children"];
		if (val && val.IsSequence())
		{
			for (auto it = val.begin(); it != val.end(); it++)
			{
				const auto& item = *it;
				Children.push_back(GUID::Parse(item.as<std::string>()));
			}
		}
	}
}