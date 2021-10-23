#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Scripting/ScriptModuleField.h"

namespace Chroma
{
	class CSharpScript : public Component
	{
	public:
		CSharpScript();
		~CSharpScript();
		CSharpScript(const CSharpScript& other) = default;

		const std::string Name() const override 
		{ 
			return StaticName();
		}

		const static std::string StaticName()
		{
			return "CSharp Script";
		}

		std::string ModuleName = "Test.TestScript";
		ScriptModuleFieldMap ModuleFieldMap;
	};
}