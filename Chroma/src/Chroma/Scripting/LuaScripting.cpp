#include "chromapch.h"

#include "LuaScripting.h"
#include <Chroma/Scene/Entity.h>
#include <lcode.h>
#include <Chroma/Components/LuaScript.h>
#include <Chroma/Core/Input.h>

namespace Chroma
{
	sol::state LuaScripting::Lua;
	std::hash<std::string> LuaScripting::hash = std::hash<std::string>();
	std::map<std::string, sol::function> LuaScripting::scripts;

	void LuaScripting::Init()
	{
		CHROMA_CORE_TRACE("Initializing Lua State...");

		Lua.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::bit32, sol::lib::utf8);
		Bindings::BindAllTheThings(&Lua);
		Lua["Input"] = Chroma::Input();

		auto thread = sol::thread::create(Lua.lua_state());
		auto env = sol::environment(thread.thread_state(), sol::create, Lua.globals());


		
		

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

	bool LuaScripting::LoadScriptFromFile(std::filesystem::path path, bool is_binary)
	{
		if (scripts.find(path.string()) != scripts.end())
			return false;

		sol::load_mode mode = sol::load_mode::text;
		if (is_binary)
			mode = sol::load_mode::binary;

		try
		{
			sol::load_result result = LuaScripting::Lua.load_file(path.string(), mode);
			if (result.valid())
			{
				scripts.emplace(path.string(), sol::function(result));
				return true;
			}
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

	bool LuaScripting::ReloadScriptFromFile(std::filesystem::path path, bool is_binary)
	{
		sol::load_mode mode = sol::load_mode::text;
		if (is_binary)
			mode = sol::load_mode::binary;

		try
		{
			sol::load_result result = LuaScripting::Lua.load_file(path.string(), mode);
			if (result.valid())
			{
				scripts[path.string()] = sol::function(result);
				return true;
			}
		}
		catch (const sol::error e)
		{
			CHROMA_ERROR("{}", e.what());
		}
		catch (...)
		{
			CHROMA_CORE_ERROR("Unknown error while attempting to compile the following script: {}", path.string());
		}

		return false;

	}

	bool LuaScripting::LoadScript(const std::string& name, const std::string& source)
	{
		if (scripts.find(name) != scripts.end())
			return false;

		const sol::load_mode mode = sol::load_mode::text;
		try
		{
			
			sol::load_result result = LuaScripting::Lua.load(source);
			if (result.valid())
			{
				scripts.emplace(name, sol::function(result));
				return true;
			}
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

	bool LuaScripting::ReloadScript(const std::string& name, const std::string& source)
	{

		const sol::load_mode mode = sol::load_mode::text;
		try
		{

			sol::load_result result = LuaScripting::Lua.load(source);
			if (result.valid())
			{
				scripts.emplace(name, sol::function(result));
				return true;
			}
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

	sol::optional<sol::protected_function_result> LuaScripting::ExecuteScript(const std::string& name, sol::thread& thread, sol::optional<sol::environment> env)
	{
		if (scripts.find(name) == scripts.end())
		{
			CHROMA_CORE_ERROR("Script not found!: {}", name);
			return sol::nullopt;
		}

			
		sol::set_default_state(thread.thread_state());
		auto results =  scripts[name]();
		sol::set_default_state(Lua.lua_state());

		if (env.has_value())
		{
			env.value().set_on(scripts[name]);
			scripts[name]();
		}
		
		return results;
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