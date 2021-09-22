#include "chromapch.h"
#include "ScriptWrappers.h"

#include "Chroma/Scripting/MonoScripting.h"
#include "Chroma/Scripting/ScriptEngineRegistry.h"

#include <mono/jit/jit.h>


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
}


