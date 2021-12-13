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

	const bool Entity::Valid() const
	{
		if (m_Scene == nullptr)
			return false;
		return m_Scene->Registry.valid(m_EntityID);
	}

	std::string Entity::ToPrefab()
	{
		/*
		YAML::Emitter out;
		out << YAML::BeginMap;
		for (auto& name : ECS::GetComponentNames())
		{
			if (ECS::HasComponent(name, m_EntityID, &m_Scene->Registry))
				ECS::GetComponent(name, m_EntityID, &m_Scene->Registry)->DoSerialize(out);
		}
		out << YAML::EndMap;
		return std::string(out.c_str());
		*/
		return std::string();
	}

	std::string Entity::GetName()
	{
		return "";
	}

	Entity Entity::CreateChild()
	{
		return m_Scene->NewChild(m_EntityID);
	}

	void Entity::SetName(const std::string& name)
	{
		
	}
}