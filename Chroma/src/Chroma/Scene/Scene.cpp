#include "chromapch.h"

#include "Scene.h"
#include "Entity.h"

namespace Chroma
{

	Entity Scene::NewEntity()
	{
		if (!m_FreeEntities.empty())
		{
			EntityIndex newIndex = m_FreeEntities.back();
			m_FreeEntities.pop_back();
			EntityID newID = CreateEntityId(newIndex, GetEntityVersion(m_Entities[newIndex]));
			m_Entities[newIndex] = newID;
			Entity entity = { newID, this };
			return { m_Entities[newIndex], this };
		}
		m_Entities.push_back(CreateEntityId(EntityIndex(m_Entities.size()), 0));
		return { m_Entities.back(), this };
	}

	void Scene::DestroyEntity(Entity entity)
	{
		EntityID id = entity.m_EntityID;
		EntityID newID = CreateEntityId(EntityIndex(-1), GetEntityVersion(id) + 1);
		m_Entities[GetEntityIndex(id)] = newID;
		m_FreeEntities.push_back(GetEntityIndex(id));
		for (AbstractComponentPool* pool : m_ComponentPools)
		{
			pool->RemoveAll(id);
		}
	}

	Entity Scene::ConvertIDToEntity(EntityID e, Scene& scene)
	{
		return Entity(e, &scene);
	}

	Entity Scene::GetEntity(EntityID e)
	{
		return Scene::ConvertIDToEntity(e, *this);
	}

}