#pragma once

#include "ScriptComponent.h"
#include "Chroma/Assets/Asset.h"
#include <sol/load_result.hpp>
#include <sol/state_view.hpp>	
#include <sol/thread.hpp>
#include <sol/coroutine.hpp>
#include <any>

namespace Chroma
{
	class ScriptingSystem;
	class LuaScripting;
	class Entity;

	class LuaScript : public ScriptComponent
	{
	public:

		/// @brief Constructs an empty LuaScript
		LuaScript();
		~LuaScript();
		/// @brief Constructs a new LuaScript from an existing LuaScript.
		/// @param  AudioSource to copy.
		LuaScript(const LuaScript&) = default;



		const std::string Name() const override { return "LuaScript"; }

		void Serialize(YAML::Emitter& out) override;
		void Deserialize(YAML::Node& node, EntityID id, Scene* out) override;
		void DrawImGui() override;

		void RegisterEntity(EntityID id, Scene* scene);
		void RegisterEntity(Entity entity);

		void StartCoroutine(const std::string& func, int update_step);
		void RestartCoroutine(const std::string& func);
		void StopCoroutine(const std::string& func);

		void ReloadState();
		void ReloadCoroutines();

		void LoadScript();

		sol::environment Environment;
		bool Success = false;
		
	private:


		friend class ScriptingSystem;
		friend class LuaScripting;

		struct Coroutine
		{
			~Coroutine() { Handle.abandon(); }
			std::string Name;
			sol::coroutine Handle;
			//sol::environment Thread;
			double Time = 0;
			int Step;
		};

		std::map<std::string, Coroutine> Coroutines;
	};
}