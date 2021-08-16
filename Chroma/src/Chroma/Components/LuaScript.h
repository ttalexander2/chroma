#pragma once

#include "ScriptComponent.h"
#include "Chroma/Assets/Asset.h"
#include <sol/load_result.hpp>
#include <sol/environment.hpp>

namespace Chroma
{
	class ScriptingSystem;

	class LuaScript : public ScriptComponent
	{
	public:
		const std::string Name() const override { return "LuaScript"; }

		void Serialize(YAML::Emitter& out) override;
		void Deserialize(YAML::Node& node) override;
		void DrawImGui() override;
		
	private:
		sol::environment env;
		bool success = false;

		friend class ScriptingSystem;
	};
}