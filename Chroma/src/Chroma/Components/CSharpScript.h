#pragma once

#include "ScriptComponent.h"
#include "Chroma/Scripting/ScriptModuleField.h"

namespace Chroma
{
	class CSharpScript : public ScriptComponent
	{
	public:
		CSharpScript();
		~CSharpScript();
		CSharpScript(const CSharpScript& other) = default;

		const std::string Name() const override { return "CSharp Script"; }

		void DrawImGui() override;

		std::string ModuleName = "Test.TestScript";
		ScriptModuleFieldMap ModuleFieldMap;
	};
}