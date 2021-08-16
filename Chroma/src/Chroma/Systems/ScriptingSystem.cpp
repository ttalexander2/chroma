#include "chromapch.h"
#include "ScriptingSystem.h"

#include <Chroma/Scripting/LuaScripting.h>
#include <Chroma/Components/LuaScript.h>
#include <Chroma/Scene/Entity.h>

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
			script.ScriptName = std::filesystem::path(script.Path).filename().string();
			script.env = sol::environment(LuaScripting::Lua, sol::create, LuaScripting::Lua.globals());
			script.env["entity"] = Entity(entity, m_Scene);
			try
			{
				auto result = LuaScripting::Lua.safe_script_file(script.Path, script.env, sol::load_mode::text);
				if (result.valid())
					script.success = true;
			}
			catch (const sol::error& e)
			{
				CHROMA_CORE_ERROR("{}", e.what());
			}
		}
	}

	void ScriptingSystem::PostLoad()
	{
	}

	void ScriptingSystem::EarlyInit()
	{
		auto view = m_Scene->Registry.view<LuaScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<LuaScript>(entity);
			auto func = script.env["EarlyInit"];
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
			auto func = script.env["Init"];
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
			auto func = script.env["LateInit"];
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
			auto func = script.env["EarlyUpdate"];
			if (func.valid())
				func();
		}
	}

	void ScriptingSystem::Update(Time time)
	{
		auto view = m_Scene->Registry.view<LuaScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<LuaScript>(entity);
			auto func = script.env["Update"];
			if (func.valid())
				func();
		}
	}

	void ScriptingSystem::LateUpdate(Time time)
	{
		auto view = m_Scene->Registry.view<LuaScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<LuaScript>(entity);
			auto func = script.env["LateUpdate"];
			if (func.valid())
				func();
		}
	}

}

