#include "chromapch.h"
#include "ScriptEngineRegistry.h"
#include <Chroma/Scene/Entity.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "Chroma/Components/AudioSource.h"
#include "Chroma/Components/BoxCollider2D.h"
#include "Chroma/Components/CircleCollider2D.h"
#include "Chroma/Components/SpriteRenderer.h"
#include "Chroma/Components/Transform.h"

namespace Chroma
{
	std::unordered_map<MonoType*, std::function<bool(Entity&)>> hasComponentFuncs;
	std::unordered_map<MonoType*, std::function<void(Entity&)>> createComponentFuncs;

	extern MonoImage* coreAssemblyImage;

#define Component_RegisterType(Type) \
	{\
		MonoType* type = mono_reflection_type_from_name((char*)"Chroma." #Type, coreAssemblyImage);\
		if (type) {\
			uint32_t id = mono_type_get_type(type);\
			hasComponentFuncs[type] = [](Entity& entity) { return entity.HasComponent<Type>();  };\
			createComponentFuncs[type] = [](Entity& entity) { return entity.AddComponent<Type>(); };\
		} else {\
			CHROMA_CORE_ERROR("No C# component class found for " #Type "!");\
		}\
	}

	static void InitComponentTypes()
	{
		Component_RegisterType(AudioSource);
		Component_RegisterType(BoxCollider2D);
		Component_RegisterType(CircleCollider2D);
		Component_RegisterType(SpriteRenderer);
		Component_RegisterType(Transform);
	}

	void ScriptEngineRegistry::RegisterAll()
	{
		InitComponentTypes();
		

	}

}