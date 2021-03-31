#pragma once

#include "ECS.h"
#include "Entity.h"

namespace Chroma
{


	template <class T>
	class Component
	{
		friend class Scene;
	public:
		T* operator->() { return m_Ptr; }

		bool IsNull() { return m_Ptr == nullptr || m_Scene == nullptr;  }
	private:
		Component<typename T>(T* ptr, Scene* scene)
			: m_Ptr(ptr), m_Scene(scene)
		{
		}
	private:
		T* m_Ptr;
		Scene* m_Scene;
	};

}

