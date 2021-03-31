#include "chromapch.h"

#include "Scene.h"

namespace Chroma
{

	EntityID Scene::NewEntity()
	{
		if (!m_FreeEntities.empty())
		{
			EntityIndex newIndex = m_FreeEntities.back();
			m_FreeEntities.pop_back();
			EntityID newID = CreateEntityId(newIndex, GetEntityVersion(m_Entities[newIndex]));
			m_Entities[newIndex] = newID;
			return m_Entities[newIndex];
		}
		m_Entities.push_back(CreateEntityId(EntityIndex(m_Entities.size()), 0));
		return m_Entities.back();
	}

	void Scene::DestroyEntity(EntityID id)
	{
		EntityID newID = CreateEntityId(EntityIndex(-1), GetEntityVersion(id) + 1);
		m_Entities[GetEntityIndex(id)] = newID;
		m_FreeEntities.push_back(GetEntityIndex(id));
		for (AbstractComponentPool* pool : m_ComponentPools)
		{
			pool->RemoveAll(id);
		}
	}



}