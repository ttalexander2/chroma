#include "chromapch.h"
#include "Entity.h"
#include "Scene.h"

#include "Chroma/Components/Tag.h"

namespace Chroma
{
	Entity::Entity(EntityID id)
		: m_EntityID(id)
	{
	}

	Entity::Entity(EntityID id, Scene* scene)
		: m_EntityID(id), m_Scene(scene)
	{
	}

	std::string Entity::ToPrefab()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		for (auto& name : ECS::GetComponentNames())
		{
			if (ECS::HasComponent(name, m_EntityID, &m_Scene->Registry))
				ECS::GetComponent(name, m_EntityID, &m_Scene->Registry)->DoSerialize(out);
		}
		out << YAML::EndMap;
		return std::string(out.c_str());
	}

	std::string Entity::GetName()
	{
		if (!HasComponent<Tag>())
			return std::string("Null");
		auto &t = GetComponent<Tag>();

		return t.EntityName;
	}

	void Entity::SetName(const std::string& name)
	{
		if (HasComponent<Tag>())
			GetComponent<Tag>().EntityName = name;
	}
}