#pragma once

#include "ScriptComponent.h"
#include "Chroma/Assets/Asset.h"
#include <sol/load_result.hpp>
#include <sol/environment.hpp>

namespace Chroma
{
	class ScriptingSystem;
	class LuaScripting;

	class LuaScript : public ScriptComponent
	{
	public:

		/// @brief Constructs an empty LuaScript
		LuaScript() = default;
		/// @brief Constructs a new LuaScript from an existing LuaScript.
		/// @param  AudioSource to copy.
		LuaScript(const LuaScript&) = default;

		const std::string Name() const override { return "LuaScript"; }

		void Serialize(YAML::Emitter& out) override;
		void Deserialize(YAML::Node& node, uint32_t id, Scene* out) override;
		void DrawImGui() override;
		
	private:
		sol::environment env;
		bool success = false;

		friend class ScriptingSystem;
		friend class LuaScripting;
	};
}