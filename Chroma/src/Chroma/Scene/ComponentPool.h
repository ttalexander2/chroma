#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>

#include "ECS.h"
#include "Component.h"

namespace Chroma
{
	class AbstractComponentPool
	{
	public:
		virtual void RemoveAll(EntityID id) = 0;
		virtual void Remove(Component* comp) = 0;
		virtual void Repack() = 0;
		inline virtual size_t Count() = 0;
		virtual bool HasEntity(EntityID id) = 0;
		virtual std::vector<EntityID>* GetEntities() = 0;
		virtual std::vector<Component*> GetAbstractComponents(EntityID id) = 0;
		virtual Component* Add(EntityID id) = 0;

	};


	template <class T>
	class ComponentPool : public AbstractComponentPool
	{
		friend class Scene;

	public:
		ComponentPool<typename T>()
		{
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
			m_SparseComponents[id].push_back((int)m_PackedComponents.size() - 1);

			m_PackedComponents.back().SetComparisonID();
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
				if (m_PackedComponents.size() > i)
					results.push_back(&m_PackedComponents[i]);
			}

			return results;
		}

		std::vector<Component*> GetAbstractComponents(EntityID id) override
		{
			std::vector<Component*> results;
			for (uint32_t i : m_SparseComponents[id])
			{
				results.push_back(static_cast<Component*>(&m_PackedComponents[i]));
			}

			return results;
		}

		void RemoveAll(EntityID id) override
		{
			if (m_SparseComponents.find(id) == m_SparseComponents.end())
				return;

			m_SparseComponents.erase(id);

			int i = 0;
			std::vector<EntityID>::iterator it = m_PackedEntityIds.begin();
			while (it != m_PackedEntityIds.end())
			{
				if (*it == id)
				{
					it = m_PackedEntityIds.erase(it);
					m_PackedComponents.erase(m_PackedComponents.begin() + i);
				}
				else
				{
					it++;
					i++;
				}
			}

			Repack();

		}

		void Remove(Component* component) override
		{

			int toRemove = -1;
			for (size_t i = m_PackedComponents.size() - 1; i >= 0; i--)
			{

				if (m_PackedComponents[i] == *component)
				{
					m_PackedComponents.erase(m_PackedComponents.begin() + i);
					toRemove = static_cast<int>(i);
					break;
				}

			}

			if (toRemove == -1)
				return;

			m_PackedEntityIds.erase(m_PackedEntityIds.begin() + toRemove);
			Repack();

		}

		void Remove(T* component)
		{

			int toRemove = -1;
			for (int i = m_PackedEntityIds.size() - 1; i >= 0; i--)
			{
				if (m_PackedComponents[i] == *component)
				{
					m_PackedComponents.erase(m_PackedComponents.begin() + i);
					toRemove = i;
					break;
				}

			}

			if (toRemove == -1)
				return;

			m_PackedEntityIds.erase(m_PackedEntityIds.begin() + toRemove);
			Repack();

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
			return m_SparseComponents.find(id) != m_SparseComponents.end() && m_SparseComponents[id].size() > 0;
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

}

