#include "chromapch.h"
#include "ScriptWrappers.h"

#include "Chroma/Scripting/MonoScripting.h"
#include "Chroma/Scripting/ScriptEngineRegistry.h"

#include <mono/jit/jit.h>
#include <Chroma/Components/Tag.h>
#include <Chroma/Components/Transform.h>
#include <Chroma/Components/SpriteRenderer.h>


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
			return mono_string_new(mono_domain_get(), sr->Layer.c_str());
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
			sr->Layer = mono_string_to_utf8(val);
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
			return mono_string_new(mono_domain_get(), sr->GetSpriteID().c_str());
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
			sr->SetSprite(mono_string_to_utf8(val));
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
}


