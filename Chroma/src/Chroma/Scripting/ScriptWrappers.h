#pragma once
#include "Chroma/Scene/ECS.h"
#include "Chroma/Core/Log.h"

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
		void TransformComponent_SetPosition(EntityID id, Math::vec2 val);


	}
}