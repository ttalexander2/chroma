#pragma once

#include "ECS.h"
#include "Scene.h"
#include "spdlog/fmt/ostr.h"

namespace Chroma
{

	class EntityRef
	{
	public:
		EntityRef(EntityID id);
		EntityRef(EntityID id, Scene& scene);
		EntityRef(const EntityRef& other) = default;

		inline EntityID GetID() const { return m_EntityID; }

		std::string ToPrefab();


		template <typename T>
		bool HasComponent()
		{
			return m_Scene->HasComponent<T>(m_EntityID);
		}

		bool HasComponent(unsigned int component_id)
		{
			return m_Scene->HasComponent(m_EntityID, component_id);
		}

		template<typename T>
		ComponentRef<T> AddComponent()
		{
			return m_Scene->AddComponent<T>(m_EntityID);
		}

		template<typename T>
		ComponentRef<T> GetComponent()
		{
			return m_Scene->GetComponent<T>(m_EntityID);
		}

		template<typename T>
		std::vector<ComponentRef<T>> GetComponents()
		{
			return m_Scene->GetComponents<T>(m_EntityID);
		}

		std::vector<ComponentRef<Component>> GetAllComponents();

		template<typename T>
		void RemoveComponents()
		{
			RemoveComponents<T>(m_EntityID);
		}

		template<typename T>
		void RemoveComponent(ComponentRef<T> component)
		{
			RemoveComponent<T>(component);
		}

		explicit operator EntityID() const { return m_EntityID; }
		operator bool() const { return m_EntityID != ENTITY_NULL; }

		bool operator==(const EntityRef& e) const
		{
			return (this->m_EntityID == e.m_EntityID && this->m_Scene == e.m_Scene);
		}

		bool operator<(const EntityRef& b) const
		{
			return (this->m_EntityID < b.m_EntityID);
		}

		template<typename OStream>
		friend OStream& operator<<(OStream& os, const EntityRef& e)
		{
			return os << "Entity " << e.m_EntityID << "";
		}


	private:

		EntityID m_EntityID { ENTITY_NULL };
		Ref<Scene> m_Scene;

		friend class Scene;
	};
}

