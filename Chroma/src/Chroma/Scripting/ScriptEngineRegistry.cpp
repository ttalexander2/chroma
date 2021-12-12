#include "chromapch.h"
#include "ScriptEngineRegistry.h"
#include <Chroma/Scene/Entity.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "Chroma/Components/AudioSource.h"
#include "Chroma/Components/BoxCollider.h"
#include "Chroma/Components/CircleCollider.h"
#include "Chroma/Components/SpriteRenderer.h"
#include "Chroma/Components/Transform.h"
#include "Chroma/Components/Tag.h"
#include "Chroma/Components/Camera.h"
#include "Chroma/Components/ParticleEmitter.h"
#include <Chroma/Scripting/ScriptWrappers.h>

#include "MonoScripting.cpp"

namespace Chroma
{
	std::unordered_map<MonoType*, std::function<bool(Entity&)>> hasComponentFuncs;
	std::unordered_map<MonoType*, std::function<void(Entity&)>> createComponentFuncs;

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
		Component_RegisterType(Transform);
		Component_RegisterType(AudioSource);
		Component_RegisterType(BoxCollider);
		Component_RegisterType(CircleCollider);
		Component_RegisterType(SpriteRenderer);
		Component_RegisterType(Tag);
		Component_RegisterType(Camera);
		Component_RegisterType(ParticleEmitter);
	}

	void ScriptEngineRegistry::RegisterAll()
	{
		InitComponentTypes();

		//Entity
		mono_add_internal_call("Chroma.Entity::CreateComponent_Native", Chroma::Script::Entity_CreateComponent);
		mono_add_internal_call("Chroma.Entity::HasComponent_Native", Chroma::Script::Entity_HasComponent);
		mono_add_internal_call("Chroma.Entity::FindEntityByName_Native", Chroma::Script::Entity_FindEntityByName);
		mono_add_internal_call("Chroma.Entity::GetChildren_Native", Chroma::Script::Entity_GetChildrenNative);
		mono_add_internal_call("Chroma.Entity::GetChildByName_Native", Chroma::Script::Entity_GetChildByNameNative);
		mono_add_internal_call("Chroma.Entity::GetFirstChild_Native", Chroma::Script::Entity_GetFirstChildNative);
		mono_add_internal_call("Chroma.Entity::HasChildren_Native", Chroma::Script::Entity_HasChildrenNative);
		mono_add_internal_call("Chroma.Entity::NumChildren_Native", Chroma::Script::Entity_NumChildrenNative);
		mono_add_internal_call("Chroma.Entity::GetAbsolutePosition_Native", Chroma::Script::Entity_GetAbsolutePositionNative);
		mono_add_internal_call("Chroma.Entity::SetAbsolutePosition_Native", Chroma::Script::Entity_SetAbsolutePositionNative);


		//Log
		mono_add_internal_call("Chroma.Log::LogMessage_Native", Chroma::Script::Log_Message);

		//Tag
		mono_add_internal_call("Chroma.Tag::GetTag_Native", Chroma::Script::TagComponent_GetTag);
		mono_add_internal_call("Chroma.Tag::SetTag_Native", Chroma::Script::TagComponent_SetTag);

		//Transform
		mono_add_internal_call("Chroma.Transform::GetTransformPosition_Native", Chroma::Script::TransformComponent_GetPosition);
		mono_add_internal_call("Chroma.Transform::GetTransformRotation_Native", Chroma::Script::TransformComponent_GetRotation);
		mono_add_internal_call("Chroma.Transform::GetTransformScale_Native", Chroma::Script::TransformComponent_GetScale);
		mono_add_internal_call("Chroma.Transform::SetTransformPosition_Native", Chroma::Script::TransformComponent_SetPosition);
		mono_add_internal_call("Chroma.Transform::SetTransformRotation_Native", Chroma::Script::TransformComponent_SetRotation);
		mono_add_internal_call("Chroma.Transform::SetTransformScale_Native", Chroma::Script::TransformComponent_SetScale);

		//Input
		mono_add_internal_call("Chroma.Input::IsKeyPressed_Native", Chroma::Input::IsKeyPressed);
		mono_add_internal_call("Chroma.Input::IsMouseButtonPressed_Native", Chroma::Input::IsMouseButtonPressed);
		mono_add_internal_call("Chroma.Input::IsJoystickConnected_Native", Chroma::Input::IsJoystickConnected);
		mono_add_internal_call("Chroma.Input::GetMousePosition_Native", Chroma::Script::Input_GetMousePosition);
		mono_add_internal_call("Chroma.Input::GetFirstConnectedGamepad_Native", Chroma::Input::GetFirstConnectedGamepad);
		mono_add_internal_call("Chroma.Input::GetAllConnectedControllers", Chroma::Script::Input_GetAllConnectedControllers);
		mono_add_internal_call("Chroma.Input::SetPrimaryGamepad_Native", Chroma::Input::SetPrimaryGamepad);
		mono_add_internal_call("Chroma.Input::GetGamepadAxis_Native", Chroma::Script::Input_GetGamepadAxis);
		mono_add_internal_call("Chroma.Input::GetGamepadButtonPressed_Native", Chroma::Script::Input_GetGamepadButtonState);
		mono_add_internal_call("Chroma.Input::GetGamepadConnectionState_Native", Chroma::Input::GetGamepadConnectionState);


		//Sprite Renderer
		mono_add_internal_call("Chroma.SpriteRenderer::GetColor_Native", Chroma::Script::SpriteRenderer_GetColor);
		mono_add_internal_call("Chroma.SpriteRenderer::SetColor_Native", Chroma::Script::SpriteRenderer_SetColor);
		mono_add_internal_call("Chroma.SpriteRenderer::GetOffset_Native", Chroma::Script::SpriteRenderer_GetOffset);
		mono_add_internal_call("Chroma.SpriteRenderer::SetOffset_Native", Chroma::Script::SpriteRenderer_SetOffset);
		mono_add_internal_call("Chroma.SpriteRenderer::GetLayer_Native", Chroma::Script::SpriteRenderer_GetLayer);
		mono_add_internal_call("Chroma.SpriteRenderer::SetLayer_Native", Chroma::Script::SpriteRenderer_SetLayer);
		mono_add_internal_call("Chroma.SpriteRenderer::GetPlayOnStart_Native", Chroma::Script::SpriteRenderer_GetPlayOnStart);
		mono_add_internal_call("Chroma.SpriteRenderer::SetPlayOnStart_Native", Chroma::Script::SpriteRenderer_SetPlayOnStart);
		mono_add_internal_call("Chroma.SpriteRenderer::GetPlaying_Native", Chroma::Script::SpriteRenderer_GetPlaying);
		mono_add_internal_call("Chroma.SpriteRenderer::SetPlaying_Native", Chroma::Script::SpriteRenderer_SetPlaying);
		mono_add_internal_call("Chroma.SpriteRenderer::GetLoop_Native", Chroma::Script::SpriteRenderer_GetLoop);
		mono_add_internal_call("Chroma.SpriteRenderer::SetLoop_Native", Chroma::Script::SpriteRenderer_SetLoop);
		mono_add_internal_call("Chroma.SpriteRenderer::GetSpeedMultiplier_Native", Chroma::Script::SpriteRenderer_GetSpeedMultiplier);
		mono_add_internal_call("Chroma.SpriteRenderer::SetSpeedMultiplier_Native", Chroma::Script::SpriteRenderer_SetSpeedMultiplier);
		mono_add_internal_call("Chroma.SpriteRenderer::GetSprite_Native", Chroma::Script::SpriteRenderer_GetSprite);
		mono_add_internal_call("Chroma.SpriteRenderer::SetSprite_Native", Chroma::Script::SpriteRenderer_SetSprite);
		mono_add_internal_call("Chroma.SpriteRenderer::GetAnimation_Native", Chroma::Script::SpriteRenderer_GetAnimation);
		mono_add_internal_call("Chroma.SpriteRenderer::SetAnimation_Native", Chroma::Script::SpriteRenderer_SetAnimation);
		mono_add_internal_call("Chroma.SpriteRenderer::GetFrame_Native", Chroma::Script::SpriteRenderer_GetFrame);
		mono_add_internal_call("Chroma.SpriteRenderer::SetFrame_Native", Chroma::Script::SpriteRenderer_SetFrame);
		mono_add_internal_call("Chroma.SpriteRenderer::GetAnimationTag_Native", Chroma::Script::SpriteRenderer_GetAnimationTag);
		mono_add_internal_call("Chroma.SpriteRenderer::SetAnimationByTag_Native", Chroma::Script::SpriteRenderer_SetAnimationByTag);
		mono_add_internal_call("Chroma.SpriteRenderer::RestartAnimation_Native", Chroma::Script::SpriteRenderer_RestartAnimation);


		//Audio
		mono_add_internal_call("Chroma.Audio::PlayEvent_Native", Chroma::Script::Audio_PlayEvent);
		mono_add_internal_call("Chroma.Audio::PlayEventIfStopped_Native", Chroma::Script::Audio_PlayEventIfStopped);
		mono_add_internal_call("Chroma.Audio::StopEvent_Native", Chroma::Script::Audio_StopEvent);
		mono_add_internal_call("Chroma.Audio::GetEventParameter_Native", Chroma::Script::Audio_GetEventParameter);
		mono_add_internal_call("Chroma.Audio::SetEventParameter_Native", Chroma::Script::Audio_SetEventParameter);
		mono_add_internal_call("Chroma.Audio::IsEventPlaying_Native", Chroma::Script::Audio_IsEventPlaying);

	}

}