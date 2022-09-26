#pragma once

#include "EntityID.h"
#include "Chroma/Utilities/StringHash.h"
#include <unordered_map>
#include <functional>

namespace Chroma
{
	class Scene;
	struct Component;
	struct Reflection;

	struct ComponentRegistry
	{
		friend class Scene;
	public:
		template <typename T>
		static void RegisterComponent(uint32_t component_id)
		{
			//std::cout << "Registered " << component_id << "\n";
			if constexpr (std::is_abstract_v<T>)
				return;

			ComponentAddFunctions()[component_id] = [component_id](EntityID id, entt::registry *registry)
			{
				if (registry->try_get<T>(id) == nullptr)
				{
					registry->emplace<T>(id, id);
					AddRequirements(component_id, id, registry);
				}
				return dynamic_cast<Component *>(registry->try_get<T>(id));
			};

			ComponentHasFunctions()[component_id] = [](EntityID id, entt::registry *registry)
			{
				return registry->try_get<T>(id) != nullptr;
			};

			ComponentGetFunctions()[component_id] = [](EntityID id, entt::registry *registry)
			{
				return registry->try_get<T>(id);
			};

			ComponentRemoveFunctions()[component_id] = [](EntityID id, entt::registry *registry)
			{
				return registry->remove<T>(id);
			};

			ComponentCopyFunctions()[component_id] = [](EntityID id, entt::registry *dst, entt::registry *src)
			{
				dst->emplace<T>(id, src->get<T>(id));
			};

			ComponentEmplaceOrReplaceFunctions()[component_id] = [](EntityID id, Component *val, entt::registry *registry)
			{
				if (val == nullptr)
				{
					registry->emplace_or_replace<T>(id, id);
				}
				T comp = *static_cast<T *>(val);
				registry->emplace_or_replace<T>(id, comp);
				return true;
			};
		}

		template <typename T>
		static bool IsComponentRegistered()
		{
			const BasicHash<uint32_t> hash = T::GetTypeInfoStatic()->GetType();
			return ComponentAddFunctions().contains(hash);
		}

		static bool IsComponentRegistered(const std::string &component_name)
		{
			const BasicHash<uint32_t> hash = BasicHash<uint32_t>::Hash(component_name.c_str());
			return ComponentAddFunctions().contains(hash.m_Hash);
		}

		static Component *AddComponent(uint32_t component_id, EntityID id, entt::registry *r);
		static Component *GetComponent(uint32_t component_id, EntityID id, entt::registry *r);
		static size_t RemoveComponent(uint32_t component_id, EntityID id, entt::registry *r);
		static bool HasComponent(uint32_t component_id, EntityID id, entt::registry *r);
		static void CopyComponent(uint32_t component_id, EntityID id, entt::registry *dst, entt::registry *src);
		static Component *AddComponent(const std::string &component_name, EntityID id, entt::registry *r);
		static Component *GetComponent(const std::string &component_name, EntityID id, entt::registry *r);
		static bool HasComponent(const std::string &component_name, EntityID id, entt::registry *r);
		static size_t RemoveComponent(const std::string &component_name, EntityID id, entt::registry *r);
		static void CopyComponent(const std::string &component_name, EntityID id, entt::registry *dst, entt::registry *src);

		static bool EmplaceOrReplaceComponentFromPtr(uint32_t component_id, EntityID id, Component *val, entt::registry *r);
		static bool EmplaceOrReplaceComponentFromPtr(const std::string &component_name, EntityID id, Component *val, entt::registry *r);

		static std::unordered_map<uint32_t, std::function<Component*(EntityID, entt::registry *)>> &ComponentAddFunctions()
		{
			static std::unordered_map<uint32_t, std::function<Component*(EntityID, entt::registry *)>> s_ComponentAdd;
			return s_ComponentAdd;
		}

		static std::unordered_map<uint32_t, std::function<Component*(EntityID, entt::registry *)>> &ComponentGetFunctions()
		{
			static std::unordered_map<uint32_t, std::function<Component*(EntityID, entt::registry *)>> s_ComponentGet;
			return s_ComponentGet;
		}

		static std::unordered_map<uint32_t, std::function<bool(EntityID, entt::registry *)>> &ComponentHasFunctions()
		{
			static std::unordered_map<uint32_t, std::function<bool(EntityID, entt::registry *)>> s_ComponentHas;
			return s_ComponentHas;
		}

		static std::unordered_map<uint32_t, std::function<size_t(EntityID, entt::registry *)>> &ComponentRemoveFunctions()
		{
			static std::unordered_map<uint32_t, std::function<size_t(EntityID, entt::registry *)>> s_ComponentRemove;
			return s_ComponentRemove;
		}

		static std::unordered_map<uint32_t, std::function<void(EntityID, entt::registry *, entt::registry *)>> &ComponentCopyFunctions()
		{
			static std::unordered_map<uint32_t, std::function<void(EntityID, entt::registry *, entt::registry *)>> s_CopyComponent;
			return s_CopyComponent;
		}

		static std::unordered_map<uint32_t, std::function<bool (EntityID, Component *, entt::registry *)>> &ComponentEmplaceOrReplaceFunctions()
		{
			static std::unordered_map<uint32_t, std::function<bool (EntityID, Component *, entt::registry *)>> s_ComponentEmplaceOrReplace;
			return s_ComponentEmplaceOrReplace;
		}

	private:
		static void AddRequirements(uint32_t component_id, EntityID entity, entt::registry *r);
	};
}
