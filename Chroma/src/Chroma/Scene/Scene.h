#pragma once

#include <unordered_map>
#include <set>
#include <string>

#include "Chroma/Core/Core.h"
#include "ECS.h"
#include "ComponentRef.h"
#include "ComponentPool.h"
#include "Chroma/Core/Time.h"
#include "Component.h"

#include "Chroma/Core/Log.h"


namespace Chroma
{
	//Forward Declaration
	class EntityRef;
	class System;


	class Scene
	{
	public:
		std::string Name = "Scene";


	public:
		Scene();
		~Scene() = default;

		EntityRef NewEntity();

		void DestroyEntity(EntityRef id);

		std::string Serialize();
		static bool Deserialize(Scene& out, const std::string& yaml);

		template <typename T>
		void RegisterSystem()
		{
			//CHROMA_ASSERT(std::is_convertible<T*, System*>::value, "Must inherrit Component to register as component");

			//if (!std::is_convertible<T*, System*>::value)
			//{
			//	return;
			//}
			System* system = (System*)new T();
			system->m_Scene = this;
			m_Systems.push_back(system);
		}

		template <typename T>
		void RegisterComponent()
		{
			bool check = std::is_convertible<T*, Component*>::value;
			CHROMA_ASSERT(check, "Must inherrit Component to register as component");

			if (!check)
			{
				return;
			}


			unsigned int id = GetComponentTypeID<T>();

			m_ComponentIDs[typeid(T).hash_code()] = id;

			auto func = [](Scene& scene, EntityID entity)
			{
				ComponentRef<T> comp = scene.AddComponent<T>(entity);
				return ComponentRef<Component>(comp.m_Ptr, comp.GetEntityID(), comp.GetScene());
			};

			T test;

			if (m_ComponentFactory.find(test.Name()) == m_ComponentFactory.end())
			{
				m_ComponentFactory[test.Name()] = func;
			}

			m_ComponentNames[id] = test.Name();
			m_ComponentNamesToID[test.Name()] = id;
			m_ComponentsAllowMultiple[id] = test.AllowMultiple();

			if (m_ComponentPools.size() <= id || m_ComponentPools[id] == nullptr)
			{
				m_ComponentPools.push_back(new ComponentPool<T>());
			}

			CHROMA_CORE_TRACE("Registered Component: {0}", m_ComponentNames[id]);
		}

		template<typename T>
		ComponentRef<T> AddComponent(EntityID id)
		{
			CHROMA_ASSERT(m_ComponentIDs.find(typeid(T).hash_code()) != m_ComponentIDs.end(), "Component not registered: {}", typeid(T).name());

			//this is a little sus
			int componentID = GetComponentTypeID<T>();

			//if (m_ComponentPools.size() <= componentID)
			//{
			//	m_ComponentPools.resize(componentID + 1, nullptr);
			//}

			if (m_ComponentPools.size() <= componentID || m_ComponentPools[componentID] == nullptr)
			{
				m_ComponentPools.push_back(new ComponentPool<T>());
			}

			//Placement new operator to add new T() to address given by get(id)
			ComponentPool<T>* pool = (ComponentPool<T>*)m_ComponentPools[componentID];

			return ComponentRef<T>(pool->Add(id), id, this);
		}

		template<typename T>
		ComponentRef<T> AddComponent(EntityRef id)
		{
			return AddComponent<T>((EntityID)id);
		}

		template<typename T>
		ComponentRef<T> GetComponent(EntityID id)
		{
			int componentID = GetComponentTypeID<T>();

			if (!HasComponent(id, componentID))
				return ComponentRef<T>(nullptr, id, nullptr);

			ComponentPool<T>* pool = (ComponentPool<T>*)m_ComponentPools[componentID];
			return ComponentRef<T>(pool->GetFirst(id), id, this);
		}

		template<typename T>
		ComponentRef<T> GetComponent(EntityRef id)
		{
			return GetComponent<T>((EntityID)id);
		}

		template<typename T>
		std::vector<ComponentRef<T>> GetComponents(EntityID id)
		{

			int componentID = GetComponentTypeID<T>();

			if (!HasComponent<T>(id))
			{
				return {};
			}


			ComponentPool<T>* pool = (ComponentPool<T>*)m_ComponentPools[componentID];
			std::vector<T*> raw = pool->Get(id);
			std::vector<ComponentRef<T>> refs;
			for (T* item : raw)
			{
				refs.push_back(ComponentRef<T>(item, id, this));
			}
			return refs;
		}

		template<typename T>
		std::vector<ComponentRef<T>> GetComponents(EntityRef id)
		{
			return GetComponents<T>((EntityID)id);
		}

		template<typename T>
		bool HasComponent(EntityRef id)
		{
			return HasComponent<T>((EntityID)id);
		}

		bool HasComponent(EntityID id, unsigned int componentID)
		{
			if (m_ComponentPools.size() <= componentID || m_ComponentPools[componentID] == nullptr)
				return false;

			AbstractComponentPool* pool = m_ComponentPools[componentID];
			return pool->HasEntity(id);
		}

		template<typename T>
		bool HasComponent(EntityID id)
		{

			int componentID = GetComponentTypeID<T>();

			if (m_ComponentPools.size() <= componentID || m_ComponentPools[componentID] == nullptr)
				return false;

			AbstractComponentPool* pool = m_ComponentPools[componentID];
			return pool->HasEntity(id);
		}

		template<typename T>
		void RemoveComponents(EntityID id)
		{
			int componentId = GetComponentTypeID<T>();

			if (m_Entities.contains(id))
				return;

			if (m_ComponentPools.size() <= componentId || m_ComponentPools[componentId] == nullptr)
				return;


			ComponentPool<T>* pool = (ComponentPool<T>*)m_ComponentPools[componentId];
			pool->RemoveAll(id);
			pool->Repack();
		}

		template<typename T>
		void RemoveComponents(EntityRef id)
		{
			RemoveComponents<T>((EntityID)id);
		}

		template<typename T>
		void RemoveComponent(ComponentRef<T> component)
		{
			int componentId = GetComponentTypeID<T>();

			if (m_ComponentPools.size() <= componentId || m_ComponentPools[componentId] == nullptr)
				return;

			ComponentPool<T>* pool = (ComponentPool<T>*)m_ComponentPools[componentId];
			pool->Remove(component.m_Ptr);
			pool->Repack();
		}

		void RemoveComponent(ComponentRef<Component> component)
		{
			unsigned int componentId = m_ComponentIDs[typeid(*component.m_Ptr).hash_code()];

			if (m_ComponentPools.size() <= componentId || m_ComponentPools[componentId] == nullptr)
				return;

			AbstractComponentPool* pool = (AbstractComponentPool*)m_ComponentPools[componentId];
			pool->Remove(component.m_Ptr);
			pool->Repack();
		}

		std::vector<ComponentRef<Component>>  GetAllComponents(EntityRef entity);

		std::vector<ComponentRef<Component>>  GetAllComponents(EntityID entity);


		template<typename... ComponentTypes>
		struct SceneView
		{
			friend class Scene;

		public:
			std::set<EntityRef>::iterator begin()
			{
				return m_Entities.begin();
			}

			std::set<EntityRef>::iterator end()
			{
				return m_Entities.end();
			}

			const std::set<EntityRef>::const_iterator begin() const
			{
				return m_Entities.begin();
			}

			const std::set<EntityRef>::const_iterator end() const
			{
				return m_Entities.end();
			}

			size_t size()
			{
				return m_Entities.size();
			}

			const size_t size() const
			{
				return m_Entities.size();
			}

		private:
			SceneView(Scene& scene)
				: m_Scene(&scene)
			{
				m_Entities = std::set<EntityRef>();
				unsigned int componentIds[] = { scene.GetComponentTypeID<ComponentTypes>()... };
				size_t smallestSize = -1;
				int smallest = 0;


				for (int id : componentIds)
				{
					if (id >= m_Scene->m_ComponentPools.size()
						||
						m_Scene->m_ComponentPools[id] == nullptr)
						continue;

					AbstractComponentPool* pool = m_Scene->m_ComponentPools[id];
					if (smallestSize == -1 || (pool->Count() < smallestSize) && pool->Count() > 0)
					{
						smallest = id;
					}
				}

				for (EntityID e_id : *m_Scene->m_ComponentPools[smallest]->GetEntities())
				{
					bool valid = true;
					for (int id : componentIds)
					{
						if (id == smallest)
							continue;
						if (m_Scene->m_ComponentPools.size() <= id || m_Scene->m_ComponentPools[id] == nullptr)
							continue;

						if (!m_Scene->m_ComponentPools[id]->HasEntity(e_id))
						{
							valid = false;
							break;
						}
					}
					if (valid)
						m_Entities.emplace(Scene::ConvertIDToEntity(e_id, scene));
				}
			}


		private:
			std::set<EntityRef> m_Entities;
			Scene* m_Scene{ nullptr };
		};


		template<typename... ComponentTypes>
		SceneView<ComponentTypes...> View()
		{
			return SceneView<ComponentTypes...>(*this);
		}

		static EntityRef ConvertIDToEntity(EntityID e, Scene& scene);

		EntityRef GetEntity(EntityID e);

		std::unordered_set<EntityID> GetEntities()
		{
			return m_Entities;
		}

		template <class T>
		unsigned int GetComponentTypeID()
		{
			static unsigned int s_ComponentId = m_ComponentTypeCounter++;
			return s_ComponentId;
		}

		std::unordered_map<std::string, std::function<ComponentRef<Component>(Scene&, EntityID)>> GetComponentFactory()
		{
			return m_ComponentFactory;
		}

		std::unordered_map<unsigned int, std::string> GetComponentNames()
		{
			return m_ComponentNames;
		}

		std::unordered_map<std::string, unsigned int> GetComponentNamestoIDMap()
		{
			return m_ComponentNamesToID;
		}

		bool ComponentAllowsMultiple(unsigned int component_id)
		{
			return m_ComponentsAllowMultiple[component_id];
		}

#pragma region UpdateLoop

		void EarlyInit();
		void Init();
		void LateInit();

		void EarlyUpdate(Time delta);
		void Update(Time delta);
		void LateUpdate(Time delta);

		void EarlyDraw(Time delta);
		void Draw(Time delta);
		void LateDraw(Time delta);

#pragma endregion


	private:

		EntityRef NewEntityFromID(EntityID id);
		EntityID NewEntityID() { return m_EntityCounter++; }


	private:

		EntityID m_EntityCounter = 0;
		std::unordered_set<EntityID> m_Entities;

		std::unordered_map<size_t, unsigned int> m_ComponentIDs;
		std::unordered_map<std::string, std::function<ComponentRef<Component>(Scene&, EntityID)>> m_ComponentFactory;
		std::unordered_map<unsigned int, std::string> m_ComponentNames;
		std::unordered_map<std::string, unsigned int> m_ComponentNamesToID;
		std::unordered_map<unsigned int, bool> m_ComponentsAllowMultiple;

		std::vector<System*> m_Systems;
		std::vector<AbstractComponentPool*> m_ComponentPools;



		unsigned int m_ComponentTypeCounter = 0;

		friend class EntityRef;


	};

}