#include "chromapch.h"
#include "ComponentRegistry.h"

#include "Component.h"


namespace Chroma
{
	
	Component *ComponentRegistry::AddComponent(uint32_t component_id, EntityID id, entt::registry *r)
	{
		if (ComponentAddFunctions().find(component_id) == ComponentAddFunctions().end())
			return nullptr;
		return ComponentAddFunctions()[component_id](id, r);
	}

	Component *ComponentRegistry::GetComponent(uint32_t component_id, EntityID id, entt::registry *r)
	{
		if (ComponentGetFunctions().find(component_id) == ComponentGetFunctions().end())
			return nullptr;
		return ComponentGetFunctions()[component_id](id, r);
	}

	bool ComponentRegistry::HasComponent(uint32_t component_id, EntityID id, entt::registry *r)
	{
		if (ComponentHasFunctions().find(component_id) == ComponentHasFunctions().end())
			return false;
		return ComponentHasFunctions()[component_id](id, r);
	}

	size_t ComponentRegistry::RemoveComponent(uint32_t component_id, EntityID id, entt::registry *r)
	{
		if (ComponentRemoveFunctions().find(component_id) == ComponentRemoveFunctions().end())
			return 0u;
		return ComponentRemoveFunctions()[component_id](id, r);
	}

	void ComponentRegistry::CopyComponent(uint32_t component_id, EntityID id, entt::registry *dst, entt::registry *src)
	{
		if (ComponentCopyFunctions().find(component_id) == ComponentCopyFunctions().end())
			return;
		ComponentCopyFunctions()[component_id](id, dst, src);
	}

	Component *ComponentRegistry::AddComponent(const std::string &component_name, EntityID id, entt::registry *r)
	{
		return AddComponent(StringHash::Hash(component_name).m_Hash, id, r);
	}

	Component *ComponentRegistry::GetComponent(const std::string &component_name, EntityID id, entt::registry *r)
	{
		return GetComponent(StringHash::Hash(component_name).m_Hash, id, r);
	}

	bool ComponentRegistry::HasComponent(const std::string &component_name, EntityID id, entt::registry *r)
	{
		return HasComponent(StringHash::Hash(component_name).m_Hash, id, r);
	}

	size_t ComponentRegistry::RemoveComponent(const std::string &component_name, EntityID id, entt::registry *r)
	{
		return RemoveComponent(StringHash::Hash(component_name).m_Hash, id, r);
	}

	void ComponentRegistry::CopyComponent(const std::string &component_name, EntityID id, entt::registry *dst, entt::registry *src)
	{
		CopyComponent(StringHash::Hash(component_name).m_Hash, id, dst, src);
	}

} //namespace Chroma