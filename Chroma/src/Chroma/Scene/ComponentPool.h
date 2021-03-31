#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>

#include "ECS.h"

namespace Chroma
{
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
			return m_SparseComponents.count(id) == 1 && m_SparseComponents[id].size() > 0;
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

