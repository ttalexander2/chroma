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

	std::string EntityRef::ToPrefab()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		for (ComponentRef c : GetAllComponents())
		{
			c->BeginSerialize(out);
			c->Serialize(out);
			c->EndSerialize(out);
		}
		out << YAML::EndMap;
		return std::string(out.c_str());
	}

	std::vector<ComponentRef<Component>> EntityRef::GetAllComponents()
	{
		return m_Scene->GetAllComponents(m_EntityID);
	}
}