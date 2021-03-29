#pragma once

#include <unordered_map>
#include <string>

namespace Chroma
{

	const size_t MAX_COMPONENTS = 64;
	const size_t MIN_ENTITIES = 512;

	typedef unsigned long long EntityID;
	typedef unsigned long EntityIndex;
	typedef unsigned long EntityVersion;
	typedef unsigned int ComponentTypeID;

	/*
	* EntityID - 32 bits
	*
	*	|32 bits				| 32 bits				|
	*	|unique_ID				| version				|
	*/

	struct Entity
	{
		friend class Scene;

		EntityID id;
		std::string name;
	};


	template <class T>
	class Component
	{
	friend class Scene;
	public:
		T* operator->() { return m_Ptr; }
	private:
		Component<typename T>(T* ptr)
			: m_Ptr(ptr) {}
	private:
		T* m_Ptr;
	};




	class AbstractComponentPool
	{
	public:
		virtual void RemoveAll(EntityID id) = 0;
		virtual void Repack() = 0;
		inline virtual size_t Count() = 0;
		virtual bool HasEntity(EntityID id) = 0;
		virtual std::vector<EntityID>* GetEntities() = 0;

	};




	template <class T>
	class ComponentPool : public AbstractComponentPool
	{
		friend class Scene;

	public:
		ComponentPool<typename T>()
		{
			m_PackedComponents.reserve(sizeof(T) * MIN_ENTITIES);
		}

		inline size_t Count() override
		{
			return m_SparseComponents.size();
		}

		T* Add(EntityID id)
		{
			m_PackedEntityIds.push_back(id);
			m_PackedComponents.push_back(T());
			if (m_SparseComponents.find(id) == m_SparseComponents.end())
			{
				m_SparseComponents[id] = {};
			}
			m_SparseComponents[id].push_back(m_PackedComponents.size() - 1);
			return &m_PackedComponents[m_SparseComponents[id].back()];

		}

		T* GetFirst(EntityID id)
		{
			return &m_PackedComponents[m_SparseComponents[id][0]];
		}

		std::vector<T*> Get(EntityID id)
		{
			std::vector<T*> results;
			for (uint32_t i : m_SparseComponents[id])
			{
				results.push_back(&m_PackedComponents[i]);
			}

			return results;
		}

		void RemoveAll(EntityID id) override
		{

			if (m_SparseComponents.find(id) == m_SparseComponents.end())
				return;

			std::vector<uint32_t> toRemove;
			for (int i = m_PackedEntityIds.size() - 1; i >= 0; i--)
			{
				if (m_PackedEntityIds[i] == id)
				{
					toRemove.push_back(i);
				}
			}

			m_SparseComponents.erase(id);

			for (int i = 0; i < toRemove.size(); i++)
			{
				m_PackedEntityIds.erase(m_PackedEntityIds.begin() + toRemove[i]);
				m_PackedComponents.erase(m_PackedComponents.begin() + toRemove[i]);
			}

		}

		void Remove(T* component)
		{

			int toRemove = -1;
			for (int i = m_PackedEntityIds.size() - 1; i >= 0; i--)
			{
				if (&m_PackedComponents[i] == component)
				{
					m_PackedComponents.erase(m_PackedComponents.begin() + i);
					toRemove = i;
					break;
				}

			}

			if (toRemove == -1)
				return;

			m_PackedEntityIds.erase(m_PackedEntityIds.begin() + toRemove);

		}

		void Repack() override
		{
			m_SparseComponents.clear();
			for (int i = 0; i < m_PackedEntityIds.size(); i++)
			{
				EntityID id = m_PackedEntityIds[i];
				if (m_SparseComponents.find(id) == m_SparseComponents.end())
				{
					m_SparseComponents[id] = {};
				}
				m_SparseComponents[id].push_back(i);
			}

		}

		bool HasEntity(EntityID id) override
		{
			return m_SparseComponents.count(id) == 1;
		}

		std::vector<EntityID>* GetEntities() override
		{
			return &m_PackedEntityIds;
		}
		

	private:
		std::unordered_map<EntityID, std::vector<uint32_t>> m_SparseComponents = { };
		std::vector<EntityID> m_PackedEntityIds = { };
		std::vector<T> m_PackedComponents = { };
	};




	class Scene
	{
	public:
		Scene() {}
		~Scene() {}

		EntityID NewEntity()
		{
			if (!m_FreeEntities.empty())
			{
				EntityIndex newIndex = m_FreeEntities.back();
				m_FreeEntities.pop_back();
				EntityID newID = CreateEntityId(newIndex, GetEntityVersion(m_Entities[newIndex].id));
				m_Entities[newIndex].id = newID;
				m_Entities[newIndex].name = "";
				return m_Entities[newIndex].id;
			}
			m_Entities.push_back({ CreateEntityId(EntityIndex(m_Entities.size()), 0), "entity" });
			return m_Entities.back().id;
		}

		void DestroyEntity(EntityID id)
		{
			EntityID newID = CreateEntityId(EntityIndex(-1), GetEntityVersion(id) + 1);
			m_Entities[GetEntityIndex(id)].id = newID;
			m_FreeEntities.push_back(GetEntityIndex(id));
			for (AbstractComponentPool* pool : m_ComponentPools)
			{
				pool->RemoveAll(id);
			}
		}
		
		template<class T>
		Component<T> AddComponent(EntityID id)
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

			return *new Component<T>(pool->Add(id));
		}

		template<class T>
		Component<T> GetComponent(EntityID id)
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

			ComponentPool<T> *pool = (ComponentPool<T>*)m_ComponentPools[componentID];
			return *new Component<T>(pool->GetFirst(id));
		}

		template<class T>
		std::vector<Component<T>> GetComponents(EntityID id)
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

			ComponentPool<T>* pool = (ComponentPool<T>*)m_ComponentPools[componentID];
			std::vector<T*> raw =  pool->Get(id);
			std::vector<Component<T>> refs;
			for (T* item : raw)
			{
				refs.push_back(*new Component<T>(item));
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

		template<typename... ComponentTypes>
		struct SceneView
		{

			const std::vector<EntityID>::const_iterator begin() const
			{
				return m_Entities.begin();
			}

			const std::vector<EntityID>::const_iterator end() const
			{
				return m_Entities.end();
			}

		private:
			SceneView(Scene& scene) 
				: m_Scene(&scene)
			{
				int componentIds[] = { 0, GetComponentTypeID<ComponentTypes>() ... };
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

				for (EntityID id : m_Scene->m_ComponentPools[smallest]->GetEntities())
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
						m_Entities.push_back(id);
				}
			}

		private:
			std::vector<EntityID> m_Entities;
			Scene* m_Scene{ nullptr };
		};

		template<typename... ComponentTypes>
		SceneView<ComponentTypes...> View()
		{
			return new SceneView<ComponentTypes...>(&this);
		}

	private:

		EntityIndex m_EntityCounter = 0;
		unsigned int m_ComponentTypeCounter = 0;
		std::vector<Entity> m_Entities;
		std::vector<EntityID> m_FreeEntities;

		std::vector<AbstractComponentPool*> m_ComponentPools;

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

		
	};

}