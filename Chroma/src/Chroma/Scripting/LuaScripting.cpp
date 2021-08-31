#include "chromapch.h"

#include "LuaScripting.h"
#include <Chroma/Scene/Entity.h>
#include <lcode.h>
#include <Chroma/Components/LuaScript.h>
#include <Chroma/Core/Input.h>

namespace Chroma
{
	sol::state LuaScripting::Lua;
	std::vector<std::string> LuaScripting::Scripts;

	void LuaScripting::Init()
	{
		CHROMA_CORE_TRACE("Initializing Lua State...");

		Lua.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::bit32, sol::lib::utf8);
		Bindings::BindAllTheThings(Lua);
		Lua["Input"] = Chroma::Input();
	}

	void LuaScripting::BindState(sol::state_view& state)
	{
		state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::bit32, sol::lib::utf8);
		Bindings::BindAllTheThings(state);
		state["Input"] = Chroma::Input();
	}


	static int writer(lua_State* L, const void* p, size_t size, void* u)
	{
		UNUSED(L);
		return (fwrite(p, size, 1, (FILE*)u) != 1) && (size != 0);
	}

	bool LuaScripting::LoadScriptFromFile(std::filesystem::path path, sol::environment& env, bool is_binary)
	{
		sol::load_mode mode = sol::load_mode::text;
		if (is_binary)
			mode = sol::load_mode::binary;

		try
		{
			auto result = Lua.safe_script_file(path.string(), env, mode);
			return result.valid();

		}
		catch (const sol::error e)
		{
			CHROMA_ERROR("{}", e.what());
		}
		catch(...)
		{
			CHROMA_CORE_ERROR("Unknown error while attempting to compile the following script: {}", path.string());
		}

		return false;

	}

	bool LuaScripting::LoadScript(const std::string& name, const std::string& source, sol::environment& env)
	{

		try
		{
			auto result = Lua.safe_script(source, env);
			return result.valid();

		}
		catch (const sol::error e)
		{
			CHROMA_ERROR("{}", e.what());
		}
		catch (...)
		{
			CHROMA_ERROR("Unkown error while attempting to compile the following script: {}", name);
		}

		return false;
	}


	void LuaScripting::CompileToBytecode(std::filesystem::path script)
	{
		CompileToBytecode(script, std::filesystem::path(script).replace_extension(".luac"));
	}

	void LuaScripting::CompileToBytecode(std::filesystem::path script, std::filesystem::path output)
	{
		try
		{
			// This is really sus, but i couldn't get it to work with sol
			if (luaL_loadfile(LuaScripting::Lua.lua_state(), script.string().c_str()) == LUA_OK)
			{
				FILE* D = fopen(output.string().c_str(), "wb+");
				lua_lock(LuaScripting::Lua.lua_state());
				lua_dump(LuaScripting::Lua.lua_state(), writer, D, 0);
				lua_unlock(LuaScripting::Lua.lua_state());
				fclose(D);
			}
		}
		catch (const std::exception& e)
		{
			CHROMA_CORE_ERROR("{}", e.what());
		}
		catch (...)
		{
			CHROMA_CORE_ERROR("Unkown error while attempting to compile the following script: {}", script.string());
		}
	}
}