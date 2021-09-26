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
			out = &t->Position;
		}
	}
	void Script::TransformComponent_SetPosition(EntityID id, Math::vec2 val)
	{
	}
}


