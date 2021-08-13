#pragma once

#define SOL_ALL_SAFETIES_ON 1
#define SOL_SAFE_NUMERICS 1
#include <sol/sol.hpp>
#include <Chroma/Core/Log.h>

#include "Bindings.h"

namespace Chroma
{
	
	class LuaScripting
	{
	public:

		static void Init();

		static void Shutdown()
		{
			Lua.collect_garbage();
			delete Lua;
		}

	private:
		static sol::state Lua;

	};
}