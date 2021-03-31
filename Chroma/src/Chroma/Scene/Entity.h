#pragma once

#include "ECS.h"
#include "Component.h"

namespace Chroma
{

	class Scene;

	class Entity
	{
		friend class Scene;

	public:

		std::string name;


	private:
		Entity(EntityID entity_id, Scene* entity_scene)
			:id(entity_id), scene(entity_scene)
		{

		}

		EntityID id;
		Scene* scene;
	};

}