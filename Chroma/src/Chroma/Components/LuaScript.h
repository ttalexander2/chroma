#pragma once

#include "ScriptComponent.h"

namespace Chroma
{
	class LuaScript : public ScriptComponent
	{
	public:
		const std::string Name() const override { return "LuaScript"; }
	};
}