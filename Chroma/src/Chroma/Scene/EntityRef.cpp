#include "chromapch.h"
#include "EntityRef.h"
#include "Scene.h"

namespace Chroma
{
	EntityRef::EntityRef(EntityID id)
		: m_EntityID(id)
	{
	}

	EntityRef::EntityRef(EntityID id, Scene& scene)
		: m_EntityID(id)
	{
		m_Scene = CreateRef<Scene>(scene);
	}

	std::vector<ComponentRef<Component>> EntityRef::GetAllComponents()
	{
		return m_Scene->GetAllComponents(m_EntityID);
	}
}