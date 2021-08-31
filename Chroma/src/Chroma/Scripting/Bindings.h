#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/state.hpp>

namespace Chroma
{
	class Bindings
	{
	public:
		static void BindAllTheThings(sol::state_view& lua);

		//TODO: Separate all bindings into separate translation units to improve compile time.
		static void BindVec2(sol::state_view& lua);
		static void BindIVec2(sol::state_view& lua);
		static void BindVec3(sol::state_view& lua);
		static void BindIVec3(sol::state_view& lua);
		static void BindVec4(sol::state_view& lua);
		static void BindIVec4(sol::state_view& lua);
		static void BindMat4(sol::state_view& lua);

		static void BindMath(sol::state_view& lua);

		static void BindEntity(sol::state_view& lua);
		static void BindComponents(sol::state_view& lua);
		static void BindScene(sol::state_view& lua);

		static void BindTime(sol::state_view& lua);
		static void BindInput(sol::state_view& lua);
	};

}


