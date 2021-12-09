#pragma once
#include "Chroma/Scene/ECS.h"
#include "Chroma/Core/Log.h"
#include "Chroma/Core/Input.h"

#include <mono/jit/jit.h>


namespace Chroma
{
	namespace Script
	{
		//Entity
		void Entity_CreateComponent(EntityID id, void* type);
		bool Entity_HasComponent(EntityID id, void* type);
		uint32_t Entity_FindEntityByName(MonoString* name);

		//Log
		void Log_Message(Log::LogLevel level, MonoString* message);

		//Tag
		MonoString* TagComponent_GetTag(EntityID id);
		void TagComponent_SetTag(EntityID id, MonoString* tag);

		//Transform
		void TransformComponent_GetPosition(EntityID id, Math::vec2* out);
		float TransformComponent_GetRotation(EntityID id);
		void TransformComponent_GetScale(EntityID id, Math::vec2* out);
		void TransformComponent_SetPosition(EntityID id, Math::vec2* val);
		void TransformComponent_SetRotation(EntityID id, float val);
		void TransformComponent_SetScale(EntityID id, Math::vec2* val);

		//Input
		void Input_GetMousePosition(Math::vec2* out);
		void Input_GetAllConnectedControllers(MonoArray* out);
		void Input_GetGamepadAxis(Input::Gamepad gamepad, MonoArray* out);
		bool Input_GetGamepadButtonState(Input::Gamepad gamepad, Input::GamepadButton button);

		//Sprite Renderer

        void SpriteRenderer_GetColor(EntityID id, Math::vec4* vec);
        void SpriteRenderer_SetColor(EntityID id, Math::vec4* vec);
        void SpriteRenderer_GetOffset(EntityID id, Math::vec2* vec);
        void SpriteRenderer_SetOffset(EntityID id, Math::vec2* vec);
        MonoString* SpriteRenderer_GetLayer(EntityID id);
        void SpriteRenderer_SetLayer(EntityID id, MonoString* val);
        bool SpriteRenderer_GetPlayOnStart(EntityID id);
        void SpriteRenderer_SetPlayOnStart(EntityID id, bool val);
        bool SpriteRenderer_GetPlaying(EntityID id);
        void SpriteRenderer_SetPlaying(EntityID id, bool val);
        bool SpriteRenderer_GetLoop(EntityID id);
        void SpriteRenderer_SetLoop(EntityID id, bool val);
        float SpriteRenderer_GetSpeedMultiplier(EntityID id);
        void SpriteRenderer_SetSpeedMultiplier(EntityID id, float val);
		MonoString* SpriteRenderer_GetSprite(EntityID id);
        void SpriteRenderer_SetSprite(EntityID id, MonoString* val);
        unsigned int SpriteRenderer_GetAnimation(EntityID id);
        void SpriteRenderer_SetAnimation(EntityID id, unsigned int val);
        unsigned int SpriteRenderer_GetFrame(EntityID id);
        void SpriteRenderer_SetFrame(EntityID id, unsigned int val);
		MonoString* SpriteRenderer_GetAnimationTag(EntityID id, unsigned int val);
        void SpriteRenderer_SetAnimationByTag(EntityID id, MonoString* val);
        void SpriteRenderer_RestartAnimation(EntityID id);
		














	}
}