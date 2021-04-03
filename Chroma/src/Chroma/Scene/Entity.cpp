#include "chromapch.h"
#include "Entity.h"
#include "Scene.h"

namespace Chroma
{
	Entity::Entity(EntityID id, Scene* scene)
		: m_EntityID(id), m_Scene(scene)
	{
	}
}