#include "chromapch.h"

#include "Scene.h"
#include "EntityRef.h"
#include "Chroma/Utilities/Json.h"
#include <Chroma/Components/Transform.h>
#include <Chroma/Components/Tag.h>
#include <Chroma/Components/AudioSource.h>
#include <Chroma/Components/SpriteRenderer.h>
#include <Chroma/Components/BoxCollider2D.h>
#include <Chroma/Components/CircleCollider2D.h>


namespace Chroma
{
	Scene::Scene()
	{
		RegisterComponent<Tag>();
		RegisterComponent<Transform>();
		RegisterComponent<AudioSource>();
		RegisterComponent<SpriteRenderer>();
		RegisterComponent<BoxCollider2D>();
		RegisterComponent<CircleCollider2D>();
	}

	EntityRef Scene::NewEntity()
	{
		if (!m_FreeEntities.empty())
		{
			EntityIndex newIndex = m_FreeEntities.back();
			m_FreeEntities.pop_back();
			EntityID newID = CreateEntityId(newIndex, GetEntityVersion(m_Entities[newIndex]));
			m_Entities[newIndex] = newID;
			EntityRef entity = { newID, *this };
			return { m_Entities[newIndex], *this };
		}
		m_Entities.push_back(CreateEntityId(EntityIndex(m_Entities.size()), 0));
		EntityRef e = { m_Entities.back(), *this };
		std::string name = "Entity_" + std::to_string(GetEntityIndex(e.m_EntityID));
		e.AddComponent<Tag>()->EntityName = name;
		e.AddComponent<Transform>();
		return { m_Entities.back(), *this };
	}

	void Scene::DestroyEntity(EntityRef entity)
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

	std::vector<ComponentRef<Component>> Scene::GetAllComponents(EntityRef entity)
	{
		return Scene::GetAllComponents(entity.GetID());
	}

	std::vector<ComponentRef<Component>> Scene::GetAllComponents(EntityID id)
	{
		if (this == nullptr)
			return std::vector<ComponentRef<Component>>();

		std::vector<ComponentRef<Component>> result;
		for (AbstractComponentPool* pool : m_ComponentPools)
		{
			if (pool == nullptr)
				continue;

			for (Component* c : pool->GetAbstractComponents(id))
			{
				result.push_back(ComponentRef<Component>(c, id, this));
			}
		}

		return result;
	}

	EntityRef Scene::ConvertIDToEntity(EntityID e, Scene& scene)
	{
		return EntityRef(e, scene);
	}

	EntityRef Scene::GetEntity(EntityID e)
	{
		return Scene::ConvertIDToEntity(e, *this);
	}


	void Serialize()
	{
	}

	void Scene::EarlyInit()
	{
	}

	void Scene::Init()
	{
	}

	void Scene::LateInit()
	{
	}

	void Scene::EarlyUpdate(Time delta)
	{
	}

	void Scene::Update(Time delta)
	{
	}

	void Scene::LateUpdate(Time delta)
	{
	}

}