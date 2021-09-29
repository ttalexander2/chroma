#pragma once

#include <unordered_map>
#include <set>
#include <string>

#include "Chroma/Core/Core.h"
#include "ECS.h"
#include "Chroma/Core/Time.h"
#include "Component.h"

#include "Chroma/Core/Log.h"
#include "Chroma/Profiler/Instrumentor.h"

namespace Polychrome
{
	class Hierarchy;
	class Inspector;
}

namespace Chroma
{
	//Forward Declaration
	class Entity;
	class System;


	//This entire class is a template-caused mess
	class Scene
	{
		friend class ECS;
	public:
		std::string Name = "Scene";

		const GUID GetID();

		Scene();
		Scene(const Scene&) {}
		~Scene() = default;

		Entity NewEntity();
		Entity NewChild(Entity id);

		void DestroyEntity(EntityID id, bool destroy_children = false);

		std::string Serialize();
		static bool Deserialize(Scene* out, const std::string& yaml);

		bool IsDescendant(EntityID child, EntityID parent);
		bool IsRoot(EntityID entity);
		EntityID GetRootEntity(EntityID child);
		std::vector<EntityID> FindAllDescendants(EntityID entity);
		


		template <typename T>
		void RegisterSystem()
		{
			bool result = std::is_base_of<System, T>();
			CHROMA_ASSERT(result, std::string(typeid(T).name()) + " must inherrit System to register as a system.");

			if (!result)
			{
				return;
			}
			System* system = (System*)new T();
			system->m_Scene = this;
			Systems.push_back(system);
		}


		template<ComponentType T>
		T& AddComponent(EntityID id)
		{
			if (!ECS::IsComponentRegistered<T>())
			{
				ECS::RegisterComponent<T>();
			}
			
			auto& comp = Registry.emplace<T>(id);
			return comp;
		}

		Component* AddComponent(const std::string& component, EntityID entity)
		{
			return ECS::AddComponent(component, entity, &Registry);
		}

		template<ComponentType T>
		T& GetComponent(EntityID id)
		{
			return Registry.get_or_emplace<T>(id);
		}

		Component* GetComponent(const std::string& component, EntityID entity)
		{
			return ECS::GetComponent(component, entity, &Registry);
		}

		/*
		template<ComponentType T>
		std::vector<ComponentRef<T>> GetComponents(EntityID id)
		{

		}

		template<ComponentType T>
		std::vector<ComponentRef<T>> GetComponents(Entity id)
		{
			return GetComponents<T>((EntityID)id);
		}

		*/

		template<ComponentType T>
		bool HasComponent(EntityID id)
		{
			return Registry.try_get<T>(id) != nullptr;
		}

		bool HasComponent(const std::string& component, EntityID entity)
		{
			return ECS::HasComponent(component, entity, &Registry);
		}

		/*
		template<ComponentType T>
		void RemoveComponents(EntityID id)
		{
			//Recursively check
		}

		template<ComponentType T>
		void RemoveComponents(Entity id)
		{
			RemoveComponents<T>((EntityID)id);
		}
		*/

		template<ComponentType T>
		size_t RemoveComponent(EntityID entity)
		{
			return Registry.remove<T>(entity);
		}

		size_t RemoveComponent(const std::string& component, EntityID entity)
		{
			return ECS::RemoveComponent(component, entity, &Registry);
		}

		std::vector<Component*> GetAllComponents(EntityID entity);


		//template<ComponentType... Types>
		//auto View() { return Registry.view<Types>(); }

#pragma region UpdateLoop

		//TODO: Load & Unload Functions

		void PreLoad();
		void Load();
		void PostLoad();

		void EarlyInit();
		void Init();
		void LateInit();

		void EarlyUpdate(Time delta);
		void Update(Time delta);
		void LateUpdate(Time delta);

		void PreDraw(Time delta);
		void Draw(Time delta);
		void PostDraw(Time delta);

#pragma endregion
		entt::registry Registry;

	private:
		std::vector<System*> Systems;
		std::vector<EntityID> EntityOrder;

		GUID ID;

		friend class Entity;
		friend class Polychrome::Hierarchy;
		friend class Polychrome::Inspector;


	};

}