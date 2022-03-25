#include "chromapch.h"
#include "CSharpScript.h"

namespace Chroma
{

	void CSharpScript::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Module";
		out << YAML::Value << ModuleName;
	}

	void CSharpScript::Deserialize(YAML::Node& node)
	{
		auto val = node["Module"];
		if (val)
		{
			ModuleName = val.as<std::string>();
		}
	}

	void CSharpScript::CreateReflectionModel()
	{
		entt::meta<CSharpScript>()
			.data<&CSharpScript::ModuleName>("ModuleName"_hs)
			.type("CSharpScript"_hs);
	}
}