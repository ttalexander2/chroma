#include "chromapch.h"
#include "LuaScript.h"

#include "Chroma/Scene/Scene.h"
#include "Chroma/Scripting/LuaScripting.h"
#include "Chroma/Scene/Entity.h"
#include "Chroma/Scene/ECS.h"



namespace Chroma
{

	std::hash<std::string> _hash = std::hash<std::string>();

	LuaScript::LuaScript()
	{
		Environment = sol::environment(LuaScripting::Lua.lua_state(), sol::create, LuaScripting::Lua.globals());
		Environment.set_function("start_coroutine", [&](const std::string& func) { StartCoroutine(func, 1);  });
		Environment.set_function("restart_coroutine", [&](const std::string& func) { RestartCoroutine(func); });
		Environment.set_function("stop_coroutine", [&](const std::string& func) { StopCoroutine(func); });
	}

	LuaScript::~LuaScript()
	{
		Coroutines.clear();
	}

	void LuaScript::ReloadState()
	{
		Environment.set_function("start_coroutine", [&](const std::string& func) { StartCoroutine(func, 1);  });
		Environment.set_function("restart_coroutine", [&](const std::string& func) { RestartCoroutine(func); });
		Environment.set_function("stop_coroutine", [&](const std::string& func) { StopCoroutine(func); });

	}

	void LuaScript::ReloadCoroutines()
	{
		for (auto& [name, co] : Coroutines)
		{
			//co.Handle = co.Thread[co.Name];
		}
	}

	void LuaScript::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Path";
		out << YAML::Value << Path;
		if (Success)
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
			ScriptName = std::filesystem::path(Path).filename().string();
		}
	}


	void LuaScript::LoadScript()
	{
		//this->Environment = sol::environment(this->Thread.thread_state(), sol::create, LuaScripting::Lua.globals());
		this->Success = LuaScripting::LoadScriptFromFile(this->Path, this->Environment);
	}




	void LuaScript::RegisterEntity(EntityID id, Scene* scene)
	{
		Environment["entity"] = Entity(id, scene);
	}

	void LuaScript::RegisterEntity(Entity entity)
	{
		Environment["entity"] = entity;
	}

	void LuaScript::StartCoroutine(const std::string& func, int update_step)
	{
		if (Coroutines.find(func) != Coroutines.end())
			return;
		Coroutine c;
		c.Name = func;
		c.Time = 0;
		//c.Thread = sol::environment(Environment.lua_state(), sol::create);
		c.Handle = Environment[func];
		c.Step = update_step;
		Coroutines.emplace(func, c);
	}
	void LuaScript::RestartCoroutine(const std::string& func)
	{
		if (Coroutines.find(func) == Coroutines.end())
			return;
		Coroutines[func].Time = 0;
		Coroutines[func].Handle = Environment[func];
	}
	void LuaScript::StopCoroutine(const std::string& func)
	{
		Coroutines.erase(func);
	}
}


