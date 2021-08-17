#pragma once

#define SOL_ALL_SAFETIES_ON 1
#define SOL_SAFE_NUMERICS 1
#include <sol/sol.hpp>
#include <Chroma/Core/Log.h>

#include "Bindings.h"
#include <Chroma/Scene/ECS.h>

namespace Chroma
{

	class LuaScript;
	class Scene;
	
	class LuaScripting
	{
	public:

		static sol::state Lua;

		static void Init();

		static void LoadScript(LuaScript* script, Chroma::Scene* scene = nullptr, EntityID id = ENTITY_NULL);

		static void Shutdown()
		{
			Lua.collect_garbage();
			delete Lua;
		}


	};
}