#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>

#include "ECS.h"
#include "Entity.h"
#include "Component.h"
#include "ComponentPool.h"

namespace Chroma
{

	class Scene
	{
	public:
		Scene() {}
		~Scene() {}

		EntityID NewEntity();

		void DestroyEntity(EntityID id);
		
		template<typename T>
		Component<T> CreateComponent(EntityID id)
		{
			int componentID = GetComponentTypeID<T>();

			if (m_ComponentPools.size() <= componentID)
			{
				m_ComponentPools.resize(componentID + 1, nullptr);
			}

			if (m_ComponentPools[componentID] == nullptr)
			{

				m_ComponentPools[componentID] = new ComponentPool<T>();
			}

			//Placement new operator to add new T() to address given by get(id)
			ComponentPool<T> *pool = (ComponentPool<T>*)m_ComponentPools[componentID];

			return *new Component<T>(pool->Add(id), this);
		}

		template<typename T>
		Component<T> GetComponent(EntityID id)
		{
			int componentID = GetComponentTypeID<T>();

			if (!HasComponent<T>(id))
				return *new Component<T>(nullptr, nullptr);

			ComponentPool<T> *pool = (ComponentPool<T>*)m_ComponentPools[componentID];
			return *new Component<T>(pool->GetFirst(id), this);
		}

		template<typename T>
		std::vector<Component<T>> GetComponents(EntityID id)
		{
			int componentID = GetComponentTypeID<T>();

			if (!HasComponent<T>(id))
			{
				return {};
			}


			ComponentPool<T>* pool = (ComponentPool<T>*)m_ComponentPools[componentID];
			std::vector<T*> raw =  pool->Get(id);
			std::vector<Component<T>> refs;
			for (T* item : raw)
			{
				refs.push_back(*new Component<T>(item, this));
			}
			return refs;
		}

		template<typename T>
		void RemoveComponents(EntityID id)
		{
			if (m_Entities[GetEntityIndex(id)].id != id)
				return;

			int componentId = GetComponentTypeID<T>();
			ComponentPool<T>* pool = (ComponentPool<T>*)m_ComponentPools[componentId];
			pool->RemoveAll(id);
			pool->Repack();
		}

		template<typename T>
		void RemoveComponent(Component<T> component)
		{
			int componentId = GetComponentTypeID<T>();
			ComponentPool<T>* pool = (ComponentPool<T>*)m_ComponentPools[componentId];
			pool->Remove(component.m_Ptr);
			pool->Repack();
		}

		template<typename T>
		bool HasComponent(EntityID id)
		{
			int componentID = GetComponentTypeID<T>();

			if (m_ComponentPools[componentID] == nullptr)
				return false;

			ComponentPool<T>* pool = (ComponentPool<T>*)m_ComponentPools[componentID];
			return pool->HasEntity(id);
		}


		template<typename... ComponentTypes>
		struct SceneView
		{
			friend class Scene;

		public:
			std::unordered_set<EntityID>::iterator begin()
			{
				return m_Entities.begin();
			}

			std::unordered_set<EntityID>::iterator end()
			{
				return m_Entities.end();
			}

			const std::unordered_set<EntityID>::const_iterator begin() const
			{
				return m_Entities.begin();
			}

			const std::unordered_set<EntityID>::const_iterator end() const
			{
				return m_Entities.end();
			}

		private:
			SceneView(Scene& scene) 
				: m_Scene(&scene)
			{
				int componentIds[] = { scene.GetComponentTypeID<ComponentTypes>()... };
				size_t smallestSize = -1;
				int smallest = 0;
				for (int id : componentIds)
				{
					AbstractComponentPool* pool = m_Scene->m_ComponentPools[id];
					if (pool->Count() < smallestSize || smallestSize == -1)
					{
						smallest = id;
					}
				}

				for (EntityID id : *m_Scene->m_ComponentPools[smallest]->GetEntities())
				{
					bool valid = true;
					for (int id : componentIds)
					{
						if (id == smallest)
							continue;
						if (!m_Scene->m_ComponentPools[id]->HasEntity(id))
						{
							valid = false;
							break;
						}
					}
					if (valid)
						m_Entities.emplace(id);
				}
			}

		private:
			std::unordered_set<EntityID> m_Entities;
			Scene* m_Scene{ nullptr };
		};


		template<typename... ComponentTypes>
		SceneView<ComponentTypes...> View()
		{
			return *new SceneView<ComponentTypes...>(*this);
		}

	private:

		inline EntityID CreateEntityId(EntityIndex index, EntityVersion version)
		{
			return ((EntityID)index) | ((EntityID)version << 32);
		}

		inline EntityIndex GetEntityIndex(EntityID id)
		{
			return (EntityIndex)id;
		}

		inline EntityVersion GetEntityVersion(EntityID id)
		{
			return id >> 32;
		}

		inline bool IsEntityValid(EntityID id)
		{
			return (id >> 32) != EntityIndex(-1);
		}

		#define INVALID_ENTITY CreateEntityID(EntityIndex(-1), 0)

		template <class T>
		int GetComponentTypeID()
		{
			static unsigned int s_ComponentId = m_ComponentTypeCounter++;
			return s_ComponentId;
		}


	private:

		EntityIndex m_EntityCounter = 0;
		unsigned int m_ComponentTypeCounter = 0;
		std::vector<EntityID> m_Entities;
		std::vector<EntityID> m_FreeEntities;

		std::vector<AbstractComponentPool*> m_ComponentPools;

		
	};

}