#pragma once

#include "ECS.h"
#include "Scene.h"
#include "spdlog/fmt/ostr.h"

namespace Chroma
{

	class Entity
	{
	public:
		Entity(EntityID id, Scene* scene);
		Entity(const Entity& other) = default;

		inline EntityID GetID() const { return m_EntityID; }


		template <typename T>
		bool HasComponent()
		{
			return m_Scene->HasComponent<T>(m_EntityID);
		}

		template<typename T>
		Component<T> AddComponent()
		{
			return m_Scene->AddComponent<T>(m_EntityID);
		}

		template<typename T>
		Component<T> GetComponent()
		{
			return m_Scene->GetComponent<T>(m_EntityID);
		}

		template<typename T>
		std::vector<Component<T>> GetComponents()
		{
			return m_Scene->GetComponents<T>(m_EntityID);
		}

		template<typename T>
		void RemoveComponents()
		{
			RemoveComponents<T>(m_EntityID);
		}

		template<typename T>
		void RemoveComponent(Component<T> component)
		{
			RemoveComponent<T>(component);
		}

		explicit operator EntityID() const { return m_EntityID; }
		operator bool() const { return m_EntityID != ENTITY_NULL; }

		bool operator==(const Entity& e) const
		{
			return (this->m_EntityID == e.m_EntityID && this->m_Scene == e.m_Scene);
		}

		template<typename OStream>
		friend OStream& operator<<(OStream& os, const Entity& e)
		{
			return os << "Entity " << e.m_EntityID << "";
		}


	private:
		EntityID m_EntityID{ ENTITY_NULL };
		Scene* m_Scene = nullptr;

		friend class Scene;
	};
}

