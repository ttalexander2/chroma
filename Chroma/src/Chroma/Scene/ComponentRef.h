#pragma once

#include "ECS.h"
#include "Scene.h"
#include "spdlog/fmt/ostr.h"
#include "Component.h"

namespace Chroma
{
	//Forward Declarations
	class EntityRef;
	class Component;

	template<typename T>
	concept ComponentType = std::is_base_of<Component, T>::value;

	template <class ComponentType>
	class ComponentRef
	{
		friend class Scene;
	public:
		ComponentType* operator->() { return m_Ptr; }
		Scene* GetScene() { return m_Scene; }
		EntityID GetEntityID() { return m_Entity; }

		bool IsNull() { return m_Ptr == nullptr || m_Scene == nullptr; }

		template<typename OStream>
		friend OStream& operator<<(OStream& os, const ComponentRef<ComponentType>& t)
		{
			return os << "Component: " << FindTypeName(typeid(ComponentType).name()) << " (Entity " << t.m_Entity << ")";
		}

		void Delete()
		{
			m_Scene->RemoveComponent(*this);
		}


	private:
		ComponentRef<typename ComponentType>(ComponentType* ptr, EntityID id, Scene* scene)
			: m_Ptr(ptr), m_Entity(id), m_Scene(scene)
		{
		}

		static std::string FindTypeName(const std::string& str)
		{
			size_t i = str.find_last_of("::");
			if (i >= str.size())
				return str;
			return str.substr(i + 1);
		}

	private:
		EntityID m_Entity;
		ComponentType* m_Ptr;
		Scene* m_Scene;

	};


}