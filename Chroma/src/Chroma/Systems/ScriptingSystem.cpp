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

	static int writer(lua_State* L, const void* p, size_t size, void* u)
	{
		UNUSED(L);
		return (fwrite(p, size, 1, (FILE*)u) != 1) && (size != 0);
	}

	//TODO: This probably should be handled by the asset manager, not the system.
	// Or at the very least should go through the asset manager.
	// This could get very slow if many entities have the same script.
	// After the scene is loaded, the asset systen should load all of the scripts
	// into memory, then on request, the lua environment can get the data
	// to load/execute the script in the Lua state.
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
				// This is really sus, but i couldn't get it to work with sol
				if (luaL_loadfile(LuaScripting::Lua.lua_state(), script.Path.c_str()) == LUA_OK)
				{
					FILE* D = fopen(std::filesystem::path(script.Path).replace_extension(".luac").string().c_str(), "wb+");
					lua_lock(LuaScripting::Lua.lua_state());
					lua_dump(LuaScripting::Lua.lua_state(), writer, D, 0);
					lua_unlock(LuaScripting::Lua.lua_state());
					fclose(D);
				}

				auto result = LuaScripting::Lua.safe_script_file(std::filesystem::path(script.Path).replace_extension(".luac").string(), script.env, sol::load_mode::binary);
				if (result.valid())
				{
					script.success = true;
				}
					
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
			script.env["time"] = time;
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
			script.env["time"] = time;
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
			script.env["time"] = time;
			if (func.valid())
				func();
		}
	}

}
