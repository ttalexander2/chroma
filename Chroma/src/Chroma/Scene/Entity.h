#pragma once

#include "Scene.h"
#include "spdlog/fmt/ostr.h"
#include "Inspectable.h"

namespace Chroma
{
	class Bindings;

	class Entity
	{
	public:
		Entity() = default;
		Entity(EntityID id);
		Entity(EntityID id, Scene *scene);
		Entity(const Entity &other) = default;

		EntityID GetID() const { return m_EntityID; }

		const bool Valid() const;

		std::string ToPrefab();

		std::string GetName();

		Entity CreateChild();

		void SetName(const std::string &name);

		template <ComponentType T>
		bool HasComponent()
		{
			return m_Scene->HasComponent<T>(m_EntityID);
		}

		bool HasComponent(const std::string &component)
		{
			return m_Scene->HasComponent(component, m_EntityID);
		}

		template <ComponentType T>
		T &AddComponent()
		{
			return m_Scene->AddComponent<T>(m_EntityID);
		}

		Component *AddComponent(const std::string &component)
		{
			return m_Scene->AddComponent(component, m_EntityID);
		}

		template <ComponentType T>
		T &GetComponent()
		{
			return m_Scene->GetComponent<T>(m_EntityID);
		}

		Component *GetComponent(const std::string &component)
		{
			return m_Scene->GetComponent(component, m_EntityID);
		}

		template <ComponentType T>
		size_t RemoveComponent()
		{
			return m_Scene->RemoveComponent<T>(m_EntityID);
		}

		size_t RemoveComponent(const std::string &component)
		{
			return m_Scene->RemoveComponent(component, m_EntityID);
		}

		explicit operator EntityID() const { return m_EntityID; }
		operator bool() const { return m_EntityID != ENTITY_NULL; }

		bool operator==(const Entity &e) const
		{
			return (this->m_EntityID == e.m_EntityID && this->m_Scene == e.m_Scene);
		}

		bool operator<(const Entity &b) const
		{
			return (this->m_EntityID < b.m_EntityID);
		}


		//template<typename OStream>
		//friend OStream& operator<<(OStream& os, const Entity& e)
		//{
		//	return os << "Entity " << e.m_EntityID << "";
		//}


		Scene &GetScene() { return *m_Scene; }

	private:
		EntityID m_EntityID;
		Scene *m_Scene;

		friend class Scene;
		friend class Bindings;
	};
}
