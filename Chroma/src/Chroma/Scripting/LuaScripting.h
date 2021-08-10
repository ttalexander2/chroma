#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <Chroma/Core/Log.h>

namespace Chroma
{
	
	class LuaScripting
	{
	public:

		static void Init()
		{
			CHROMA_CORE_TRACE("Initializing Lua State...");

			Lua.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::bit32, sol::lib::utf8);

			Lua.script("print('Hello, Lua!')");

		}

		static void Shutdown()
		{
			Lua.collect_garbage();
			delete Lua;
		}

	private:
		static sol::state Lua;

	};
}