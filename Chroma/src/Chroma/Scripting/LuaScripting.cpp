#include "chromapch.h"

#include "LuaScripting.h"
#include <Chroma/Scene/Entity.h>
#include <lcode.h>
#include <Chroma/Components/LuaScript.h>

namespace Chroma
{
	sol::state LuaScripting::Lua;


	void LuaScripting::Init()
	{
		CHROMA_CORE_TRACE("Initializing Lua State...");

		Lua.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::bit32, sol::lib::utf8);

		Bindings::BindAllTheThings(&Lua);
		

		//CHROMA_CORE_TRACE("x: {}, y: {}", (int)Lua["x"], (int)Lua["y"]);

		//Lua.for_each([](const sol::object& key, const sol::object& value) {
		//	CHROMA_CORE_TRACE("[{}]: {}", key.as<std::string>(), value.get_type());
		//});
	}


	static int writer(lua_State* L, const void* p, size_t size, void* u)
	{
		UNUSED(L);
		return (fwrite(p, size, 1, (FILE*)u) != 1) && (size != 0);
	}

	void LuaScripting::LoadScript(LuaScript* script, Chroma::Scene* scene, EntityID id)
	{
		script->ScriptName = std::filesystem::path(script->Path).filename().string();
		script->env = sol::environment(LuaScripting::Lua, sol::create, LuaScripting::Lua.globals());
		script->env["entity"] = Entity(id, scene);
		try
		{
			// This is really sus, but i couldn't get it to work with sol
			if (luaL_loadfile(LuaScripting::Lua.lua_state(), script->Path.c_str()) == LUA_OK)
			{
				FILE* D = fopen(std::filesystem::path(script->Path).replace_extension(".luac").string().c_str(), "wb+");
				lua_lock(LuaScripting::Lua.lua_state());
				lua_dump(LuaScripting::Lua.lua_state(), writer, D, 0);
				lua_unlock(LuaScripting::Lua.lua_state());
				fclose(D);
			}

			auto result = LuaScripting::Lua.safe_script_file(std::filesystem::path(script->Path).replace_extension(".luac").string(), script->env, sol::load_mode::binary);
			if (result.valid())
			{
				script->success = true;
			}

		}
		catch (const sol::error& e)
		{
			CHROMA_CORE_ERROR("{}", e.what());
		}
	}
}