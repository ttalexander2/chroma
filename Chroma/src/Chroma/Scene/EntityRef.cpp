#include "chromapch.h"
#include "EntityRef.h"
#include "Scene.h"

#include "Chroma/Components/Tag.h"

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

	std::string EntityRef::GetName()
	{
		auto t = GetComponent<Tag>();
		if (t.IsNull())
			return std::string("Null");
		return t->EntityName;
	}

	void EntityRef::SetName(const std::string& name)
	{
		auto t = GetComponent<Tag>();
		if (!t.IsNull())
			t->EntityName = name;
	}

	std::vector<ComponentRef<Component>> EntityRef::GetAllComponents()
	{
		return m_Scene->GetAllComponents(m_EntityID);
	}
	std::vector<Component*> EntityRef::GetAllComponents_Raw()
	{
		return m_Scene->GetAllComponents_Raw(m_EntityID);
	}
}