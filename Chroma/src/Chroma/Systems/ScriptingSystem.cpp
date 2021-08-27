#include "chromapch.h"
#include "ScriptingSystem.h"

#include <Chroma/Scripting/LuaScripting.h>
#include <Chroma/Components/LuaScript.h>
#include <Chroma/Scene/Entity.h>
#include <lua.h>
#include <lauxlib.h>

#include <lobject.h>
#include <lstate.h>
#include <lundump.h>

namespace Chroma
{
	void ScriptingSystem::PreLoad()
	{

	}

	void ScriptingSystem::Load()
	{
		auto view = m_Scene->Registry.view<LuaScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<LuaScript>(entity);
			script.Path = "assets/scripts/Test.lua";
			script.ScriptName = "Test.lua";
			LuaScripting::ReloadScriptFromFile(script.Path, false);
		}
	}

	void ScriptingSystem::PostLoad()
	{
		auto view = m_Scene->Registry.view<LuaScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<LuaScript>(entity);
			script.Thread.state()["entity"] = Entity(entity, m_Scene);
			auto result = LuaScripting::ExecuteScript(script.Path, script.Thread, script.Environment);
			if (result.has_value() && result.value().valid())
			{
				script.Success = true;
			}
		}
	}

	void ScriptingSystem::EarlyInit()
	{
		auto view = m_Scene->Registry.view<LuaScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<LuaScript>(entity);
			auto func = script.Thread.state()["EarlyInit"];
			if (func.valid())
				func();
		}
	}

	void ScriptingSystem::Init()
	{
		auto view = m_Scene->Registry.view<LuaScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<LuaScript>(entity);
			auto func = script.Thread.state()["Init"];
			if (func.valid())
				func();
		}
	}

	void ScriptingSystem::LateInit()
	{
		auto view = m_Scene->Registry.view<LuaScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<LuaScript>(entity);
			auto func = script.Thread.state()["LateInit"];
			if (func.valid())
				func();
		}
	}

	void ScriptingSystem::EarlyUpdate(Time time)
	{
		auto view = m_Scene->Registry.view<LuaScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<LuaScript>(entity);
			script.Thread.state().set_function("start_coroutine", [&](const std::string& func) { script.StartCoroutine(func, 0);  });
			auto func = script.Thread.state()["EarlyUpdate"];
			script.Thread.state()["time"] = time;
			if (func.valid())
				func();

			for (auto& [id, coroutine] : script.Coroutines)
			{
				if (coroutine.Step == 0)
				{
					if (coroutine.Time <= 0 && coroutine.Handle.valid())
					{
						sol::protected_function_result fr = coroutine.Handle();
						if (fr.valid() && fr.get_type() == sol::type::number)
						{
							coroutine.Time = fr;
						}
					}
					else
					{
						coroutine.Time -= time.GetSeconds();
					}

				}
			}
		}
	}

	void ScriptingSystem::Update(Time time)
	{
		auto view = m_Scene->Registry.view<LuaScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<LuaScript>(entity);
			if (!script.Success)
				continue;

			script.Thread.state().set_function("start_coroutine", [&](const std::string& func) { sol::coroutine c = script.Thread.state()[func]; script.StartCoroutine(func, 1);  });
			auto func = script.Thread.state()["Update"];
			script.Thread.state()["time"] = time;
			if (func.valid())
				func();

			std::vector<std::string> toRemove;

			for (auto& [id, coroutine] : script.Coroutines)
			{
				if (coroutine.Step != 2 && coroutine.Step != 0)
				{
					if (coroutine.Time <= 0 && coroutine.Handle.valid())
					{
						try
						{
							script.RegisterEntity(entity, m_Scene);
							sol::protected_function_result fr = coroutine.Handle();
							if (fr.status() == sol::call_status::ok)
							{
								toRemove.push_back(id);
								continue;
							}
							if (fr.valid() && fr.get_type() == sol::type::number)
							{
								coroutine.Time = fr;
							}
						}
						catch (sol::error& e)
						{
							CHROMA_CORE_ERROR("{}", e.what());
						}
						
					}
					else
					{
						coroutine.Time -= time.GetSeconds();
					}
				}
			}

			for (auto& id : toRemove)
			{
				script.StopCoroutine(id);
			}
		}
	}

	void ScriptingSystem::LateUpdate(Time time)
	{
		auto view = m_Scene->Registry.view<LuaScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<LuaScript>(entity);
			script.Thread.state().set_function("start_coroutine", [&](const std::string& func) { sol::coroutine c = script.Thread.state()[func]; script.StartCoroutine(func, 2);  });
			auto func = script.Thread.state()["LateUpdate"];
			script.Thread.state()["time"] = time;
			if (func.valid())
				func();

			for (auto& [id, coroutine] : script.Coroutines)
			{
				if (coroutine.Step == 2)
				{
					if (coroutine.Time <= 0 && coroutine.Handle.valid())
					{
						sol::protected_function_result fr = coroutine.Handle();
						if (fr.valid() && fr.get_type() == sol::type::number)
						{
							coroutine.Time = fr;
						}
					}
					else
					{
						coroutine.Time -= time.GetSeconds();
					}
				}
			}
		}
	}

}

