#include "chromapch.h"
#include "CSharpScript.h"

#include "Chroma/Reflection/Reflection.h"

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
		Reflection::RegisterComponent<CSharpScript>();
		Reflection::RegisterComponentProperty<CSharpScript, &CSharpScript::ModuleName>("Module");
	}
}