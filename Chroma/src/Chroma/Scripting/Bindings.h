#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/state.hpp>

namespace Chroma
{
	class Bindings
	{
	public:
		static void BindAllTheThings(sol::state* lua);


		static void BindVec2(sol::state* lua);
		static void BindIVec2(sol::state* lua);
		static void BindVec3(sol::state* lua);
		static void BindIVec3(sol::state* lua);
		static void BindVec4(sol::state* lua);
		static void BindIVec4(sol::state* lua);
		static void BindMat4(sol::state* lua);

		static void BindMath(sol::state* lua);

		static void BindEntity(sol::state* lua);
		static void BindComponents(sol::state* lua);
		static void BindScene(sol::state* lua);

		static void BindTime(sol::state* lua);
	};

}


