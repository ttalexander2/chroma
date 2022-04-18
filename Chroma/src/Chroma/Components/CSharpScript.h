#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Scripting/ScriptModuleField.h"

namespace Chroma
{
	struct CSharpScript : public Component
	{
		CHROMA_COMPONENT(CSharpScript, Component);

		void Serialize(YAML::Emitter& out) override;
		void Deserialize(YAML::Node& node) override;

		std::string ModuleName;
		ScriptModuleFieldMap ModuleFieldMap;
		int ExecutionOrder = 0;
	};
}