#include "chromapch.h"
#include "LuaScript.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include "Chroma/ImGui/Widgets/VecWithLabels.h"
#include "Chroma/Scene/Scene.h"
#include "Chroma/Scripting/LuaScripting.h"
#include "Chroma/Scene/Entity.h"
#include "Chroma/Scene/ECS.h"



namespace Chroma
{

	std::hash<std::string> _hash = std::hash<std::string>();

	LuaScript::LuaScript()
	{
		Thread = sol::thread::create(LuaScripting::Lua.lua_state());
		Environment = sol::environment(LuaScripting::Lua.lua_state(), sol::create, LuaScripting::Lua.globals());
		Thread.state().set_function("start_coroutine", [&](const std::string& func) { StartCoroutine(func, 1);  });
		Thread.state().set_function("restart_coroutine", [&](const std::string& func) { RestartCoroutine(func); });
		Thread.state().set_function("stop_coroutine", [&](const std::string& func) { StopCoroutine(func); });
	}

	LuaScript::~LuaScript()
	{
		Thread.abandon();
		Coroutines.clear();
	}

	void LuaScript::ReloadState()
	{
		Thread.state().set_function("start_coroutine", [&](const std::string& func) { StartCoroutine(func, 1);  });
		Thread.state().set_function("restart_coroutine", [&](const std::string& func) { RestartCoroutine(func); });
		Thread.state().set_function("stop_coroutine", [&](const std::string& func) { StopCoroutine(func); });

	}

	void LuaScript::ReloadCoroutines()
	{
		for (auto& [name, co] : Coroutines)
		{
			co.Handle = co.Thread.state()[co.Name];
		}
	}

	void LuaScript::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Path";
		out << YAML::Value << Path;
		if (Success && Thread.valid())
		{
			/*
			Environment.for_each([&](const sol::object& key, const sol::object& value) {
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
						CHROMA_CORE_INFO("unknown value lmao {}", value.get_type());
					}
				}
			});
			*/
		}
	}

	void LuaScript::Deserialize(YAML::Node& node, EntityID id, Scene* out)
	{
		auto val = node["Path"];
		if (val)
		{
			Path = val.as<std::string>();
		}

		
	}

	void LuaScript::DrawImGui()
	{
		if (Success && Thread.valid())
		{
			Environment.for_each([&](const sol::object& key, const sol::object& value) {
				if (!value.is<sol::function>())
				{
					std::string name = key.as<std::string>();
					if (name == "entity" || name == "time")
						return;
					std::string hash = fmt::format("##{}", name);

					DrawComponentValue(name);
					if (value.is<bool>())
					{
						bool val = Thread.state()[name];
						ImGui::Checkbox(hash.c_str(), &val);
						Thread.state()[name] = val;
					}
					else if (value.is<int>())
					{
						int val = Thread.state()[name];
						ImGui::InputInt(hash.c_str(), &val);
						Thread.state()[name] = val;
					}
					else if (value.is<float>())
					{
						float val = Thread.state()[name];
						ImGui::InputFloat(hash.c_str(), &val);
						Thread.state()[name] = val;
					}
					else if (value.is<double>())
					{
						double val = Thread.state()[name];
						ImGui::InputDouble(hash.c_str(), &val);
						Thread.state()[name] = val;
					}
					else if (value.is<std::string>())
					{
						std::string val = Thread.state()[name];
						ImGui::InputText(hash.c_str(), &val);
						Thread.state()[name] = val;
					}
					else if (value.is<Math::vec2>())
					{
						Math::vec2 val = Thread.state()[name];
						ImGui::Vec2FloatWithLabels(hash.c_str(), val, true);
						Thread.state()[name] = val;
					}
					else if (value.is<Math::vec3>())
					{
						Math::vec3 val = Thread.state()[name];
						ImGui::Vec3FloatWithLabels(hash.c_str(), val, true);
						Thread.state()[name] = val;
					}
				}
			});
		}
	}


	void LuaScript::RegisterEntity(EntityID id, Scene* scene)
	{
		Thread.state()["entity"] = Entity(id, scene);
	}

	void LuaScript::RegisterEntity(Entity entity)
	{
		Thread.state()["entity"] = entity;
	}

	void LuaScript::StartCoroutine(const std::string& func, int update_step)
	{
		if (Coroutines.find(func) != Coroutines.end())
			return;
		Coroutine c;
		c.Name = func;
		c.Time = 0;
		c.Thread = sol::thread::create(Thread.thread_state());
		c.Handle = c.Thread.state()[func];
		c.Step = update_step;
		Coroutines.emplace(func, c);
	}
	void LuaScript::RestartCoroutine(const std::string& func)
	{
		if (Coroutines.find(func) == Coroutines.end())
			return;
		Coroutines[func].Time = 0;
		Coroutines[func].Handle = Thread.state()[func];
	}
	void LuaScript::StopCoroutine(const std::string& func)
	{
		Coroutines.erase(func);
	}
}


