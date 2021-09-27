#include "chromapch.h"
#include "ScriptWrappers.h"

#include "Chroma/Scripting/MonoScripting.h"
#include "Chroma/Scripting/ScriptEngineRegistry.h"

#include <mono/jit/jit.h>
#include <Chroma/Components/Tag.h>
#include <Chroma/Components/Transform.h>


namespace Chroma
{
	extern std::unordered_map<MonoType*, std::function<bool(Entity&)>> hasComponentFuncs;
	extern std::unordered_map<MonoType*, std::function<void(Entity&)>> createComponentFuncs;

	void Script::Entity_CreateComponent(EntityID id, void* type)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType* monoType = mono_reflection_type_get_type((MonoReflectionType*)type);
		createComponentFuncs[monoType](e);
	}

	bool Script::Entity_HasComponent(EntityID id, void* type)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType* monoType = mono_reflection_type_get_type((MonoReflectionType*)type);
		hasComponentFuncs[monoType](e);
	}

	void Script::Log_Message(Log::LogLevel level, MonoString* message)
	{
		char* msg = mono_string_to_utf8(message);
		switch (level)
		{
		case Log::LogLevel::Trace:
			CHROMA_TRACE(msg);
			break;
		case Log::LogLevel::Debug:
			CHROMA_TRACE(msg);
			break;
		case Log::LogLevel::Info:
			CHROMA_INFO(msg);
			break;
		case Log::LogLevel::Warn:
			CHROMA_WARN(msg);
			break;
		case Log::LogLevel::Error:
			CHROMA_ERROR(msg);
			break;
		case Log::LogLevel::Critical:
			CHROMA_CRITICAL(msg);
			break;
		}
	}
	MonoString* Script::TagComponent_GetTag(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		Tag* tag = scene->Registry.try_get<Tag>(id);
		if (tag != nullptr)
		{
			return mono_string_new(mono_domain_get(), tag->EntityName.c_str());
		}
		return nullptr;

	}
	void Script::TagComponent_SetTag(EntityID id, MonoString* tag)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		Tag* t = scene->Registry.try_get<Tag>(id);
		if (t != nullptr)
		{
			t->EntityName = mono_string_to_utf8(tag);
		}
	}
	void Script::TransformComponent_GetPosition(EntityID id, Math::vec2* out)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		Transform* t = scene->Registry.try_get<Transform>(id);
		if (t != nullptr)
		{
			*out = t->Position;
		}
	}
	float Script::TransformComponent_GetRotation(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		Transform* t = scene->Registry.try_get<Transform>(id);
		if (t != nullptr)
		{
			return t->Rotation;
		}
		return -1.f;
	}
	void Script::TransformComponent_GetScale(EntityID id, Math::vec2* out)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		Transform* t = scene->Registry.try_get<Transform>(id);
		if (t != nullptr)
		{
			*out = t->Scale;
		}
	}
	void Script::TransformComponent_SetPosition(EntityID id, Math::vec2 val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		Transform* t = scene->Registry.try_get<Transform>(id);
		if (t != nullptr)
		{
			t->Position = val;
		}
	}
	void Script::TransformComponent_SetRotation(EntityID id, float val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		Transform* t = scene->Registry.try_get<Transform>(id);
		if (t != nullptr)
		{
			t->Rotation = val;
		}
	}
	void Script::TransformComponent_SetScale(EntityID id, Math::vec2 val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		Transform* t = scene->Registry.try_get<Transform>(id);
		if (t != nullptr)
		{
			t->Scale = val;
		}
	}
	void Script::Input_GetMousePosition(Math::vec2* out)
	{
		auto pair = Input::GetMousePos();
		out->x = pair.first;
		out->y = pair.second;
	}
	void Script::Input_GetAllConnectedControllers(MonoArray* out)
	{
		auto vec = Input::GetAllConnectedControllers();
		int i = 0;
		for (auto val : vec)
		{
			mono_array_set(out, Chroma::Input::Gamepad, i++, val);
		}
	}
	void Script::Input_GetGamepadAxis(Input::Gamepad gamepad, MonoArray* out)
	{
		auto vec = Chroma::Input::GetGamepadAxis(gamepad);
		int i = 0;
		for (auto val : vec)
		{
			mono_array_set(out, float, i++, val);
		}
	}

	bool Script::Input_GetGamepadButtonState(Input::Gamepad gamepad, Input::GamepadButton button)
	{
		return Chroma::Input::GetGamepadButtonState(button, gamepad) == Chroma::Input::ButtonState::PRESSED;

	}
}


