#include "chromapch.h"
#include "ComponentRegistry.h"

#include "Component.h"
#include "Chroma/Reflection/Reflection.h"


namespace Chroma
{
	Component *ComponentRegistry::AddComponent(uint32_t component_id, EntityID id, entt::registry *r)
	{
		if (!ComponentAddFunctions().contains(component_id))
			return nullptr;
		return ComponentAddFunctions()[component_id](id, r);
	}

	Component *ComponentRegistry::GetComponent(uint32_t component_id, EntityID id, entt::registry *r)
	{
		if (!ComponentGetFunctions().contains(component_id))
			return nullptr;
		return ComponentGetFunctions()[component_id](id, r);
	}

	bool ComponentRegistry::HasComponent(uint32_t component_id, EntityID id, entt::registry *r)
	{
		if (!ComponentHasFunctions().contains(component_id))
			return false;
		return ComponentHasFunctions()[component_id](id, r);
	}

	size_t ComponentRegistry::RemoveComponent(uint32_t component_id, EntityID id, entt::registry *r)
	{
		if (!ComponentRemoveFunctions().contains(component_id))
			return 0u;
		return ComponentRemoveFunctions()[component_id](id, r);
	}

	void ComponentRegistry::CopyComponent(uint32_t component_id, EntityID id, entt::registry *dst, entt::registry *src)
	{
		if (!ComponentCopyFunctions().contains(component_id))
			return;
		ComponentCopyFunctions()[component_id](id, dst, src);
	}

	Component *ComponentRegistry::AddComponent(const std::string &component_name, EntityID id, entt::registry *r)
	{
		return AddComponent(BasicHash<uint32_t>::Hash(component_name).m_Hash, id, r);
	}

	Component *ComponentRegistry::GetComponent(const std::string &component_name, EntityID id, entt::registry *r)
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
		if (!ComponentEmplaceOrReplaceFunctions().contains(component_id))
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

		//for (auto comp : Reflection::GetTypeRequirements(component_id))
		//{
		//	if (ComponentAddFunctions().contains(comp))
		//		ComponentAddFunctions()[comp](entity, r);
		//}
	}
} //namespace Chroma
