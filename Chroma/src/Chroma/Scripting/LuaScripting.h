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
		friend class Bindings;
		friend class LuaScript;

	public:

		static sol::state Lua;

		static void Init();

		static void BindState(sol::state_view& thread);

		static bool LoadScriptFromFile(std::filesystem::path path, sol::environment& state, bool is_binary = false);
		static bool LoadScript(const std::string& name, const std::string& source, sol::environment& state);


		static void CompileToBytecode(std::filesystem::path script);
		static void CompileToBytecode(std::filesystem::path script, std::filesystem::path output);

		//For loading bytecode (without executing) we'll probably need to use a custom loader? https://github.com/ThePhD/sol2/blob/develop/examples/source/custom_reader.cpp


		static void Shutdown()
		{
			Lua.collect_garbage();
			delete Lua;
		}

		static std::vector<std::string> Scripts;

		
	};
}