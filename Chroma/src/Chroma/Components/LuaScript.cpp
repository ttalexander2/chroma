#include "chromapch.h"
#include "LuaScript.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include "Chroma/ImGui/Widgets/VecWithLabels.h"


namespace Chroma
{
	void LuaScript::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Path";
		out << YAML::Value << Path;
		if (success && env.valid())
		{
			env.for_each([&](const sol::object& key, const sol::object& value) {
				if (!value.is<sol::function>())
				{
					std::string name = key.as<std::string>();
					if (name == "entity" || name == "time")
						return;

					if (value.is<bool>())
					{
						out << YAML::Key << name;
						out << YAML::Value << value.as<bool>();
					}
					else if (value.is<int>())
					{
						out << YAML::Key << name;
						out << YAML::Value << value.as<int>();
					}
					else if (value.is<float>())
					{
						out << YAML::Key << name;
						out << YAML::Value << value.as<float>();
					}
					else if (value.is<double>())
					{
						out << YAML::Key << name;
						out << YAML::Value << value.as<double>();
					}
					else if (value.is<std::string>())
					{
						out << YAML::Key << name;
						out << YAML::Value << value.as<std::string>();
					}
					else if (value.is<Math::vec2>())
					{
						out << YAML::Key << name;
						out << YAML::Value << value.as<Math::vec2>();
					}
					else if (value.is<Math::vec3>())
					{
						out << YAML::Key << name;
						out << YAML::Value << value.as<Math::vec3>();
					}
					else
					{
						CHROMA_CORE_INFO("{}", value.get_type());
					}
				}
			});
		}
	}

	void LuaScript::Deserialize(YAML::Node& node)
	{
		auto val = node["Path"];
		if (val)
		{
			Path = val.as<std::string>();
		}

		env.for_each([&](const sol::object& key, const sol::object& value) {
			if (!value.is<sol::function>())
			{
				std::string name = key.as<std::string>();
				if (name == "entity" || name == "time")
					return;



				if (value.is<bool>())
				{
					auto val = node[name];
					if (val)
					{
						env[name] = val.as<bool>();
					}
				}
				else if (value.is<int>())
				{
					auto val = node[name];
					if (val)
					{
						env[name] = val.as<int>();
					}
				}
				else if (value.is<float>())
				{
					auto val = node[name];
					if (val)
					{
						env[name] = val.as<float>();
					}
				}
				else if (value.is<double>())
				{
					auto val = node[name];
					if (val)
					{
						env[name] = val.as<double>();
					}
				}
				else if (value.is<Math::vec2>())
				{
					auto val = node[name];
					if (val)
					{
						env[name] = val.as<Math::vec2>();
					}
				}
				else if (value.is<Math::vec3>())
				{
					auto val = node[name];
					if (val)
					{
						env[name] = val.as<Math::vec3>();
					}
				}
				else if (value.is<std::string>())
				{
					auto val = node[name];
					if (val)
					{
						env[name] = val.as<std::string>();
					}
				}
			}
			});
	}

	void LuaScript::DrawImGui()
	{
		if (success && env.valid())
		{
			env.for_each([&](const sol::object& key, const sol::object& value) {
				if (!value.is<sol::function>())
				{
					std::string name = key.as<std::string>();
					if (name == "entity" || name == "time")
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
					else if (value.is<Math::vec2>())
					{
						Math::vec2 val = value.as<Math::vec2>();
						ImGui::Vec2FloatWithLabels(hash.c_str(), val, true);
						env[name] = val;
					}
					else if (value.is<Math::vec3>())
					{
						Math::vec3 val = value.as<Math::vec3>();
						ImGui::Vec3FloatWithLabels(hash.c_str(), val, true);
						env[name] = val;
					}
				}
			});
		}
	}
}


