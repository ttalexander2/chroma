#pragma once
#include "Component.h"
#include "Chroma/Components/Transform.h"

namespace Chroma
{
	template<typename T>
	concept ComponentType = std::is_base_of<Component, T>::value;

	typedef size_t eid;
	typedef size_t ComponentTypeID;


	struct AbstractComponentPool
	{
		virtual Component* emplace(eid id) = 0;
	};

	template <ComponentType T>
	struct ComponentPool : public AbstractComponentPool
	{
		T* emplace(eid id) override
		{
			if (Components.find(id) == Components.end())
			{
				Components[id] = std::vector<T>();
			}
			Components[id].push_back(T());
			return &Components[id].back();
		}

		std::unordered_map<eid, std::vector<T>> Components;
	};

	class Registry
	{
	public:

		eid Create();
		
		template <ComponentType T>
		T& Emplace(eid entity)
		{
			ComponentTypeID componentID = GetComponentTypeID<T>();
			if (ComponentPools.find(componentID) == ComponentPools.end())
			{
				ComponentPools[componentID] = new ComponentPool<T>();
			}
			return *(T*)ComponentPools[componentID]->emplace(entity);
		}

		template <ComponentType T>
		ComponentTypeID GetComponentTypeID()
		{
			static ComponentTypeID componentID = componentTypeCounter++;
			return componentID;
		}

	private:
		eid entityCounter = 0;
		ComponentTypeID componentTypeCounter = 0;
		std::unordered_set<eid> entities;
		std::unordered_map<ComponentTypeID, AbstractComponentPool*> ComponentPools;
	};

}

