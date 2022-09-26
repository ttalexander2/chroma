#include "chromapch.h"
#include "ComponentRegistry.h"

#include "Component.h"
#include "Chroma/Reflection/Reflection.h"


namespace Chroma
{
	
	Component*  ComponentRegistry::AddComponent(uint32_t component_id, EntityID id, entt::registry *r)
	{
		if (ComponentAddFunctions().find(component_id) == ComponentAddFunctions().end())
			return nullptr;
		return ComponentAddFunctions()[component_id](id, r);
	}

	Component*  ComponentRegistry::GetComponent(uint32_t component_id, EntityID id, entt::registry *r)
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

	Component*  ComponentRegistry::AddComponent(const std::string &component_name, EntityID id, entt::registry *r)
	{
		return AddComponent(BasicHash<uint32_t>::Hash(component_name).m_Hash, id, r);
	}

	Component*  ComponentRegistry::GetComponent(const std::string &component_name, EntityID id, entt::registry *r)
	{
		return GetComponent(BasicHash<uint32_t>::Hash(component_name).m_Hash, id, r);
	}

	bool ComponentRegistry::HasComponent(const std::string &component_name, EntityID id, entt::registry *r)
	{
		return HasComponent(BasicHash<uint32_t>::Hash(component_name).m_Hash, id, r);
	}

	size_t ComponentRegistry::RemoveComponent(const std::string &component_name, EntityID id, entt::registry *r)
	{
		return RemoveComponent(BasicHash<uint32_t>::Hash(component_name).m_Hash, id, r);
	}

	void ComponentRegistry::CopyComponent(const std::string &component_name, EntityID id, entt::registry *dst, entt::registry *src)
	{
		CopyComponent(BasicHash<uint32_t>::Hash(component_name).m_Hash, id, dst, src);
	}

	bool ComponentRegistry::EmplaceOrReplaceComponentFromPtr(uint32_t component_id, EntityID id, Component *val, entt::registry *r)
	{
		if (ComponentEmplaceOrReplaceFunctions().find(component_id) == ComponentEmplaceOrReplaceFunctions().end())
			return false;
		return ComponentEmplaceOrReplaceFunctions()[component_id](id, val, r);
	}

	bool ComponentRegistry::EmplaceOrReplaceComponentFromPtr(const std::string &component_name, EntityID id, Component *val, entt::registry *r)
	{
		return EmplaceOrReplaceComponentFromPtr(BasicHash<uint32_t>::Hash(component_name), id, val, r);
	}

	void ComponentRegistry::AddRequirements(uint32_t component_id, EntityID entity, entt::registry *r)
	{
		//CHROMA_CORE_TRACE("Looking for requirements for {}", component_id);
		if (Reflection::TypeData::GetTypeRequirements().contains(component_id))
		{
			for (auto comp : Reflection::TypeData::GetTypeRequirements()[component_id])
			{
				if (ComponentRegistry::ComponentAddFunctions().contains(comp))
					ComponentRegistry::ComponentAddFunctions()[comp](entity, r);
			}
		}
	}

} //namespace Chroma