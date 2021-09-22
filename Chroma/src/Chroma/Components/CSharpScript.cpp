#include "chromapch.h"
#include "CSharpScript.h"

#include <imgui.h>
#include <imgui_stdlib.h>

namespace Chroma
{
	CSharpScript::CSharpScript()
	{

	}

	CSharpScript::~CSharpScript()
	{

	}
	void CSharpScript::DrawImGui()
	{
		if (ModuleFieldMap.find(ModuleName) == ModuleFieldMap.end())
			return;

		for (auto& [name, field] : ModuleFieldMap[ModuleName])
		{
			DrawComponentValue(name);
			const char* hash = ("##" + name).c_str();
			if (field.Type == FieldType::Float)
			{
				float val = field.GetStoredValue<float>();
				ImGui::InputFloat(hash, &val);
				field.SetStoredValue<float>(val);
			}
			else if (field.Type == FieldType::Int)
			{
				int val = field.GetStoredValue<int>();
				ImGui::InputInt(hash, &val);
				field.SetStoredValue<int>(val);
			}
			else if (field.Type == FieldType::String)
			{
				std::string val = field.GetStoredValue<const std::string&>();
				ImGui::InputText(hash, &val);
				field.SetStoredValue<const std::string&>(val);
			}
		}
		//DrawComponentValue()
	}
}