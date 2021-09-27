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

		//Log
		void Log_Message(Log::LogLevel level, MonoString* message);

		//Tag
		MonoString* TagComponent_GetTag(EntityID id);
		void TagComponent_SetTag(EntityID id, MonoString* tag);

		//Transform
		void TransformComponent_GetPosition(EntityID id, Math::vec2* out);
		float TransformComponent_GetRotation(EntityID id);
		void TransformComponent_GetScale(EntityID id, Math::vec2* out);
		void TransformComponent_SetPosition(EntityID id, Math::vec2 val);
		void TransformComponent_SetRotation(EntityID id, float val);
		void TransformComponent_SetScale(EntityID id, Math::vec2 val);

		//Input
		void Input_GetMousePosition(Math::vec2* out);
		void Input_GetAllConnectedControllers(MonoArray* out);
		void Input_GetGamepadAxis(Input::Gamepad gamepad, MonoArray* out);
		bool Input_GetGamepadButtonState(Input::Gamepad gamepad, Input::GamepadButton button);



	}
}