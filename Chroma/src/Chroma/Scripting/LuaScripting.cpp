#include "chromapch.h"

#include "LuaScripting.h"

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
}