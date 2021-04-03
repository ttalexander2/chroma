#pragma once

#include "ECS.h"
#include "Scene.h"
#include "spdlog/fmt/ostr.h"

namespace Chroma
{
	//Forward Declarations
	class Entity;

	template <class T>
	class Component
	{
		friend class Scene;
	public:
		T* operator->() { return m_Ptr; }
		Scene* GetScene() { return m_Scene; }
		EntityID GetEntityID() { return m_Entity; }

		bool IsNull() { return m_Ptr == nullptr || m_Scene == nullptr; }

		template<typename OStream>
		friend OStream& operator<<(OStream& os, const Component<T>& t)
		{
			return os << "Component: " << FindTypeName(typeid(T).name()) << " (Entity " << t.m_Entity << ")";
		}


	private:
		Component<typename T>(T* ptr, EntityID id, Scene* scene)
			: m_Ptr(ptr), m_Entity(id), m_Scene(scene)
		{
		}

		static std::string FindTypeName(const std::string& str)
		{
			size_t i = str.find_last_of("::");
			if (i >= str.size())
				return str;
			return str.substr(i+1);
		}

	private:
		EntityID m_Entity;
		T* m_Ptr;
		Scene* m_Scene;

	};


}

