#include "chromapch.h"
#include "ScriptWrappers.h"

#include "Chroma/Scripting/MonoScripting.h"
#include "Chroma/Scripting/ScriptEngineRegistry.h"

#include <mono/jit/jit.h>
#include <Chroma/Components/Tag.h>
#include <Chroma/Components/Transform.h>
#include <Chroma/Components/SpriteRenderer.h>
#include <Chroma/Audio/Audio.h>
#include <Chroma/Components/Collider.h>
#include <Chroma/Components/CircleCollider.h>
#include <Chroma/Components/EdgeCollider.h>
#include <Chroma/Components/PolygonCollider.h>
#include <Chroma/Components/RectangleCollider.h>
#include <Chroma/Components/RigidBody.h>


namespace Chroma
{
	extern std::unordered_map<MonoType*, std::function<bool(Entity&)>> hasComponentFuncs;
	extern std::unordered_map<MonoType*, std::function<void(Entity&)>> createComponentFuncs;
	extern std::unordered_map<MonoType*, std::function<Component*(Entity&)>> getComponentFuncs;

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
		if (!hasComponentFuncs.contains(monoType))
			return false;
		hasComponentFuncs[monoType](e);
		return true;
	}

	uint32_t Script::Entity_FindEntityByName(MonoString* name)
	{
		char* str_name = mono_string_to_utf8(name);

		Scene* scene = MonoScripting::GetCurrentSceneContext();
		Entity entity = scene->FindEntityByName(str_name);
		if (!entity.Valid())
			return (uint32_t)Chroma::ENTITY_NULL;
		return (uint32_t)entity.GetID();
		
	}

	MonoArray* Script::Entity_GetChildrenNative(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		std::vector<EntityID> children = scene->GetChildren(id);
		MonoArray* result = mono_array_new(mono_domain_get(), mono_get_int64_class(), children.size());
		int i = 0;
		for (auto child : children)
		{
			mono_array_set(result, uint64_t, i++, (uint64_t)child);
		}
		return result;
	}

	uint32_t Script::Entity_GetChildByNameNative(EntityID id, MonoString* name)
	{
		char* str_name = mono_string_to_utf8(name);

		Scene* scene = MonoScripting::GetCurrentSceneContext();
		return (uint32_t)scene->FindChildByName(id, str_name);
	}

	uint32_t Script::Entity_GetFirstChildNative(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		return (uint32_t)scene->GetFirstChild(id);
	}

	bool Script::Entity_HasChildrenNative(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		return scene->HasChildren(id);

	}

	unsigned int Script::Entity_NumChildrenNative(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		return (unsigned int)scene->NumChildren(id);
	}

	void Script::Entity_GetAbsolutePositionNative(EntityID id, Math::vec2* out)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		Math::vec2 pos = scene->GetTransformAbsolutePosition(id);
		*out = pos;
	}

	void Script::Entity_SetAbsolutePositionNative(EntityID id, Math::vec2* vector)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		scene->SetTransformAbsolutePosition(id, *vector);
	}

	bool Script::Component_GetEnabled(EntityID id, void* type)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType* monoType = mono_reflection_type_get_type((MonoReflectionType*)type);
		Component* comp = getComponentFuncs[monoType](e);
		return comp->IsEnabled();
	}

	void Script::Component_SetEnabled(EntityID id, void* type, bool value)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType* monoType = mono_reflection_type_get_type((MonoReflectionType*)type);
		Component* comp = getComponentFuncs[monoType](e);
		comp->SetEnabled(value);
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
	void Script::TransformComponent_SetPosition(EntityID id, Math::vec2* val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		Transform* t = scene->Registry.try_get<Transform>(id);
		if (t != nullptr)
		{
			t->Position = *val;
		}
	}
	void Script::TransformComponent_SetRotation(EntityID id, float val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		Transform* t = scene->Registry.try_get<Transform>(id);
		if (t != nullptr)
		{
			t->Rotation = Math::normalize(val, 0.f, Math::two_pi<float>());
		}
	}
	void Script::TransformComponent_SetScale(EntityID id, Math::vec2* val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		Transform* t = scene->Registry.try_get<Transform>(id);
		if (t != nullptr)
		{
			t->Scale = *val;
		}
	}
	void Script::Input_GetMousePosition(Math::vec2* out)
	{
		auto pair = Input::GetMousePos();
		out->x = pair.first;
		out->y = pair.second;
	}
	MonoArray* Script::Input_GetAllConnectedControllers()
	{
		auto vec = Input::GetAllConnectedControllers();
		MonoArray* result = mono_array_new(mono_domain_get(), mono_get_int32_class(), vec.size());
		int i = 0;
		for (auto val : vec)
		{
			mono_array_set(result, Chroma::Input::Gamepad, i++, val);
		}
		return result;
	}
	MonoArray* Script::Input_GetGamepadAxis(Input::Gamepad gamepad)
	{
		auto vec = Chroma::Input::GetGamepadAxis(gamepad);
		MonoArray* result = mono_array_new(mono_domain_get(), mono_get_single_class(), vec.size());
		int i = 0;
		for (auto val : vec)
		{
			mono_array_set(result, float, i++, val);
		}

		return result;
	}

	bool Script::Input_GetGamepadButtonState(Input::Gamepad gamepad, Input::GamepadButton button)
	{
		return Chroma::Input::GetGamepadButtonState(button, gamepad) == Chroma::Input::ButtonState::PRESSED;

	}
	void Script::SpriteRenderer_GetColor(EntityID id, Math::vec4* vec)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			*vec = sr->Color;
		}
	}
	void Script::SpriteRenderer_SetColor(EntityID id, Math::vec4* vec)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			sr->Color = *vec;
		}
	}
	void Script::SpriteRenderer_GetOffset(EntityID id, Math::vec2* vec)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			*vec = sr->Offset;
		}
	}
	void Script::SpriteRenderer_SetOffset(EntityID id, Math::vec2* vec)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			sr->Offset = *vec;
		}
	}
	MonoString* Script::SpriteRenderer_GetLayer(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			return mono_string_new(mono_domain_get(), "");
		}
		return nullptr;
	}
	void Script::SpriteRenderer_SetLayer(EntityID id, MonoString* val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
		}
	}
	bool Script::SpriteRenderer_GetPlayOnStart(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			return sr->PlayOnStart;
		}
		return false;
	}
	void Script::SpriteRenderer_SetPlayOnStart(EntityID id, bool val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			sr->PlayOnStart = val;
		}
	}
	bool Script::SpriteRenderer_GetPlaying(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			return sr->Playing;
		}
		return false;
	}
	void Script::SpriteRenderer_SetPlaying(EntityID id, bool val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			sr->Playing = val;
		}
	}
	bool Script::SpriteRenderer_GetLoop(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			return sr->Loop;
		}
		return false;
	}
	void Script::SpriteRenderer_SetLoop(EntityID id, bool val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			sr->Loop = val;
		}
	}
	float Script::SpriteRenderer_GetSpeedMultiplier(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			return sr->SpeedMultiplier;
		}
		return 1.0f;
	}
	void Script::SpriteRenderer_SetSpeedMultiplier(EntityID id, float val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			sr->SpeedMultiplier = val;
		}
	}
	MonoString* Script::SpriteRenderer_GetSprite(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			return mono_string_new(mono_domain_get(), sr->GetSpriteID().ToString().c_str());
		}
		return nullptr;
	}
	void Script::SpriteRenderer_SetSprite(EntityID id, MonoString* val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			sr->SetSprite(GUID::Parse(mono_string_to_utf8(val)));
		}
	}
	unsigned int Script::SpriteRenderer_GetAnimation(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			return sr->GetAnimation();
		}
		return 0;
	}
	void Script::SpriteRenderer_SetAnimation(EntityID id, unsigned int val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			sr->SetAnimation(val);
		}
	}
	unsigned int Script::SpriteRenderer_GetFrame(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			return sr->GetCurrentFrame();
		}
		return 0;
	}
	void Script::SpriteRenderer_SetFrame(EntityID id, unsigned int val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			sr->SetCurrentFrame(val);
		}
	}
	MonoString* Script::SpriteRenderer_GetAnimationTag(EntityID id, unsigned int val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			return mono_string_new(mono_domain_get(), sr->GetAnimationName(val).c_str());
		}
		return nullptr;
	}
	void Script::SpriteRenderer_SetAnimationByTag(EntityID id, MonoString* val)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			sr->SetAnimation(mono_string_to_utf8(val));
		}
	}
	void Script::SpriteRenderer_RestartAnimation(EntityID id)
	{
		Scene* scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene, "No active scene context!");
		SpriteRenderer* sr = scene->Registry.try_get<SpriteRenderer>(id);
		if (sr != nullptr)
		{
			sr->RestartAnimation();
		}
	}

	void Script::Audio_PlayEvent(MonoString* event_name)
	{
		Chroma::Audio::PlayEvent(mono_string_to_utf8(event_name));
	}

	void Script::Audio_PlayEventIfStopped(MonoString* event_name)
	{
		Chroma::Audio::PlayEventIfStopped(mono_string_to_utf8(event_name));
	}

	void Script::Audio_StopEvent(MonoString* event_name, bool immediate)
	{
		Chroma::Audio::StopEvent(mono_string_to_utf8(event_name), immediate);
	}

	float Script::Audio_GetEventParameter(MonoString* event_name, MonoString* parameter)
	{
		float val;
		Chroma::Audio::GetEventParameter(mono_string_to_utf8(event_name), mono_string_to_utf8(parameter),&val);
		return val;
	}

	void Script::Audio_SetEventParameter(MonoString* event_name, MonoString* parameter, float value)
	{
		Chroma::Audio::SetEventParameter(mono_string_to_utf8(event_name), mono_string_to_utf8(parameter), value);
	}

	bool Script::Audio_IsEventPlaying(MonoString* event_name)
	{
		return Chroma::Audio::IsEventPlaying(mono_string_to_utf8(event_name));
	}

	int Script::RigidBody_GetBodyType(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			return (int)rigidBody->GetBodyType();
		}
	}

	void Script::RigidBody_SetBodyType(EntityID id, int val)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->SetBodyType((Chroma::RigidBody::BodyType)val);
		}
	}

	void Script::RigidBody_GetLinearVelocity(EntityID id, Math::vec2 *vec)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			*vec = rigidBody->GetLinearVelocity();
		}
	}

	void Script::RigidBody_SetLinearVelocity(EntityID id, Math::vec2 *vec)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->SetLinearVelocity(*vec);
		}
	}

	float Script::RigidBody_GetAngularVelocity(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			return rigidBody->GetAngularVelocity();
		}
	}

	void Script::RigidBody_SetAngularVelocity(EntityID id, float val)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->SetAngularVelocity(val);
		}
	}

	float Script::RigidBody_GetLinearDamping(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			return rigidBody->GetLinearDamping();
		}
	}

	void Script::RigidBody_SetLinearDamping(EntityID id, float val)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->SetLinearDamping(val);
		}
	}

	float Script::RigidBody_GetAngularDamping(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			return rigidBody->GetAngularDamping();
		}
	}

	void Script::RigidBody_SetAngularDamping(EntityID id, float val)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->SetAngularDamping(val);
		}
	}

	bool Script::RigidBody_GetSleepingAllowed(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			return rigidBody->IsSleepingAllowed();
		}
	}

	void Script::RigidBody_SetSleepingAllowed(EntityID id, bool val)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->SetSleepingAllowed(val);
		}
	}

	bool Script::RigidBody_GetAwake(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			return rigidBody->IsAwake();
		}
	}

	void Script::RigidBody_SetAwake(EntityID id, bool val)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->SetAwake(val);
		}
	}

	bool Script::RigidBody_GetFixedRotation(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			return rigidBody->IsFixedRotation();
		}
	}

	void Script::RigidBody_SetFixedRotation(EntityID id, bool val)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->SetFixedRotation(val);
		}
	}

	bool Script::RigidBody_GetBullet(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			return rigidBody->IsBullet();
		}
	}

	void Script::RigidBody_SetBullet(EntityID id, bool val)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->SetBullet(val);
		}
	}

	float Script::RigidBody_GetGravityScale(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			return rigidBody->GetGravityScale();
		}
	}

	void Script::RigidBody_SetGravityScale(EntityID id, float val)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->SetGravityScale(val);
		}
	}

	void Script::RigidBody_ApplyForce(EntityID id, Math::vec2 *force, Math::vec2 *point, bool wake)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->ApplyForce(*force, *point, wake);
		}
	}

	void Script::RigidBody_ApplyForceToCenter(EntityID id, Math::vec2 *force, bool wake)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->ApplyForce(*force, wake);
		}
	}

	void Script::RigidBody_ApplyTorque(EntityID id, float torque, bool wake)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->ApplyTorque(torque, wake);
		}
	}

	void Script::RigidBody_ApplyLinearImpulse(EntityID id, Math::vec2 *impulse, Math::vec2 *point, bool wake)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->ApplyLinearImpulse(*impulse, *point, wake);
		}
	}

	void Script::RigidBody_ApplyLinearImpulseToCenter(EntityID id, Math::vec2 *impulse, bool wake)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->ApplyLinearImpulse(*impulse, wake);
		}
	}

	void Script::RigidBody_ApplyAngularImpulse(EntityID id, float impulse, bool wake)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RigidBody *rigidBody = scene->Registry.try_get<RigidBody>(id);
		if (rigidBody)
		{
			rigidBody->ApplyAngularImpulse(impulse, wake);
		}
	}

	unsigned int Script::Collider_GetMask(EntityID id, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		return collider->Mask;
	}

	void Script::Collider_SetMask(EntityID id, unsigned int mask, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		collider->Mask = mask;
	}

	unsigned int Script::Collider_GetLayer(EntityID id, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		return collider->Layer;
	}

	void Script::Collider_SetLayer(EntityID id, unsigned int layer, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		collider->Layer = layer;
	}

	float Script::Collider_GetFriction(EntityID id, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		return collider->GetFriction();
	}

	void Script::Collider_SetFriction(EntityID id, float friction, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		collider->SetFriction(friction);
	}

	float Script::Collider_GetRestitution(EntityID id, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		return collider->GetRestitution();
	}

	void Script::Collider_SetRestitution(EntityID id, float restitution, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		collider->SetRestitution(restitution);
	}

	float Script::Collider_GetRestitutionThreshold(EntityID id, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		return collider->GetRestitutionThreshold();
	}

	void Script::Collider_SetRestitutionThreshold(EntityID id, float threshold, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		collider->SetRestitutionThreshold(threshold);
	}

	float Script::Collider_GetDensity(EntityID id, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		return collider->GetDensity();
	}

	void Script::Collider_SetDensity(EntityID id, float density, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		collider->SetDensity(density);
	}

	bool Script::Collider_GetSensor(EntityID id, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		return collider->IsSensor();
	}

	void Script::Collider_SetSensor(EntityID id, bool sensor, void *type)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		Entity e = Entity(id, scene);
		MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);
		auto collider = reinterpret_cast<Chroma::Collider*>(getComponentFuncs[monoType](e));
		collider->SetSensor(sensor);
	}

	float Script::CircleCollider_GetRadius(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		CircleCollider* collider = scene->Registry.try_get<CircleCollider>(id);
		if (collider)
		{
			return collider->GetRadius();
		}
		return 0.f;
	}

	void Script::CircleCollider_SetRadius(EntityID id, float radius)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		CircleCollider* collider = scene->Registry.try_get<CircleCollider>(id);
		if (collider)
		{
			collider->SetRadius(radius);
		}
	}

	bool Script::EdgeCollider_GetOneSided(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		EdgeCollider* collider = scene->Registry.try_get<EdgeCollider>(id);
		if (collider)
		{
			return collider->IsOneSided();
		}
	}

	void Script::EdgeCollider_SetOneSided(EntityID id, bool oneSided)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		EdgeCollider* collider = scene->Registry.try_get<EdgeCollider>(id);
		if (collider)
		{
			collider->SetOneSided(oneSided);
		}
	}

	void Script::EdgeCollider_GetV0(EntityID id, Math::vec2 * vec)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		EdgeCollider* collider = scene->Registry.try_get<EdgeCollider>(id);
		if (collider)
		{
			*vec = collider->GetV0();
		}
	}

	void Script::EdgeCollider_SetV0(EntityID id, Math::vec2 * vec)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		EdgeCollider* collider = scene->Registry.try_get<EdgeCollider>(id);
		if (collider)
		{
			collider->SetV0(*vec);
		}
	}

	void Script::EdgeCollider_GetV1(EntityID id, Math::vec2 * vec)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		EdgeCollider* collider = scene->Registry.try_get<EdgeCollider>(id);
		if (collider)
		{
			*vec = collider->GetV1();
		}
	}

	void Script::EdgeCollider_SetV1(EntityID id, Math::vec2 * vec)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		EdgeCollider* collider = scene->Registry.try_get<EdgeCollider>(id);
		if (collider)
		{
			collider->SetV1(*vec);
		}
	}

	void Script::EdgeCollider_GetV2(EntityID id, Math::vec2 * vec)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		EdgeCollider* collider = scene->Registry.try_get<EdgeCollider>(id);
		if (collider)
		{
			*vec = collider->GetV2();
		}
	}

	void Script::EdgeCollider_SetV2(EntityID id, Math::vec2 * vec)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		EdgeCollider* collider = scene->Registry.try_get<EdgeCollider>(id);
		if (collider)
		{
			collider->SetV2(*vec);
		}
	}

	void Script::EdgeCollider_GetV3(EntityID id, Math::vec2 * vec)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		EdgeCollider* collider = scene->Registry.try_get<EdgeCollider>(id);
		if (collider)
		{
			*vec = collider->GetV3();
		}
	}

	void Script::EdgeCollider_SetV3(EntityID id, Math::vec2 * vec)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		EdgeCollider* collider = scene->Registry.try_get<EdgeCollider>(id);
		if (collider)
		{
			collider->SetV3(*vec);
		}
	}

	MonoArray* Script::PolygonCollider_GetVertices(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		PolygonCollider* collider = scene->Registry.try_get<PolygonCollider>(id);
		MonoClass *klass = MonoScripting::GetCoreClass("Chroma.Collider.Vertex");
		if (collider)
		{
			MonoArray *result = mono_array_new(mono_domain_get(), klass, collider->Count());
			auto vertices = collider->GetVertices();
			int i = 0;
			for (Collider::Vertex vertex : vertices)
			{
				mono_array_set(result, Collider::Vertex, i++, vertex);
			}

			return result;

		}
		return mono_array_new(mono_domain_get(), mono_get_int64_class(), 0);
	}

	void Script::RectangleCollider_SetSize(EntityID id, Math::vec2 *vec)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RectangleCollider *collider = scene->Registry.try_get<RectangleCollider>(id);
		if (collider)
		{
			collider->SetSize(vec->x, vec->y);
		}
	}

	void Script::RectangleCollider_GetSize(EntityID id, Math::vec2 *vec)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RectangleCollider *collider = scene->Registry.try_get<RectangleCollider>(id);
		if (collider)
		{
			*vec = collider->GetSize();
		}
	}

	float Script::RectangleCollider_GetRotation(EntityID id)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RectangleCollider *collider = scene->Registry.try_get<RectangleCollider>(id);
		if (collider)
		{
			return collider->GetRotation();
		}
	}

	void Script::RectangleCollider_SetRotation(EntityID id, float val)
	{
		Scene *scene = MonoScripting::GetCurrentSceneContext();
		CHROMA_CORE_ASSERT(scene->Registry.valid(id), "Invalid entity ID!");
		RectangleCollider *collider = scene->Registry.try_get<RectangleCollider>(id);
		if (collider)
		{
			return collider->SetRotation(val);
		}
	}
	
}


