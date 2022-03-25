#include "chromapch.h"
#include "Tag.h"


namespace Chroma
{
	void Tag::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "EntityName";
		out << YAML::Value << EntityName;
	}

	void Tag::Deserialize(YAML::Node& node)
	{
		auto val = node["EntityName"];
		if (val)
		{
			EntityName = val.as<std::string>();
		}
	}

	void Tag::CreateReflectionModel()
	{
		entt::meta<Tag>()
			.data<&Tag::EntityName>("Entity"_hs)
			.type("Tag"_hs);
	}


}

