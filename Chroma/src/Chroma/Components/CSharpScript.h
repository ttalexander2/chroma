#pragma once

#include "ScriptComponent.h"
#include "Chroma/Scripting/ScriptModuleField.h"

namespace Chroma
{
	class CSharpScript : ScriptComponent
	{
	public:
		const std::string Name() const { return "CSharpScript"; }

		std::string ModuleName;
		ScriptModuleFieldMap ModuleFieldMap;
	};
}