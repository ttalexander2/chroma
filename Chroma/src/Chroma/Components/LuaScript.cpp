#include "chromapch.h"
#include "LuaScript.h"
#include "imgui.h"
#include "imgui_stdlib.h"


namespace Chroma
{
	void LuaScript::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Path";
		out << YAML::Value << Path;
	}

	void LuaScript::Deserialize(YAML::Node& node)
	{
		auto val = node["Path"];
		if (val)
		{
			Path = val.as<std::string>();
		}
	}

	void LuaScript::DrawImGui()
	{
		if (success)
		{
			env.for_each([&](const sol::object& key, const sol::object& value) {
				if (!value.is<sol::function>())
				{
					std::string name = key.as<std::string>();
					if (name == "entity")
						return;
					std::string hash = fmt::format("##{}", name);

					DrawComponentValue(name);
					if (value.is<bool>())
					{
						bool val = value.as<bool>();
						ImGui::Checkbox(hash.c_str(), &val);
						env[name] = val;
					}
					else if (value.is<int>())
					{
						int val = value.as<int>();
						ImGui::InputInt(hash.c_str(), &val);
						env[name] = val;
					}
					else if (value.is<float>())
					{
						float val = value.as<float>();
						ImGui::InputFloat(hash.c_str(), &val);
						env[name] = val;
					}
					else if (value.is<double>())
					{
						double val = value.as<double>();
						ImGui::InputDouble(hash.c_str(), &val);
						env[name] = val;
					}
					else if (value.is<std::string>())
					{
						std::string val = value.as<std::string>();
						ImGui::InputText(hash.c_str(), &val);
						env[name] = val;
					}
				}
			});
		}
	}
}


