#pragma once
#include "Chroma/Scene/ECS.h"


namespace Chroma
{
	namespace Script
	{
		void Entity_CreateComponent(EntityID id, void* type);
		bool Entity_HasComponent(EntityID id, void* type);
	}
}