#pragma once

#include <numeric>
#include "Chroma/Utilities/GUID.h"
#include <entt.hpp>
#include "Component.h"
#include "Chroma/Components/CSharpScript.h"
#include <any>



namespace Chroma
{


	using EntityID = entt::entity;
	const EntityID ENTITY_NULL = entt::null;

	//Make sure its not abstract
	template<typename T>
	concept ComponentType = std::is_base_of<Component, T>::value;

	class ECS
	{
	public:
		template <ComponentType T>
		static void RegisterComponent()
		{	
			std::string name = T().Name();
			size_t hash = Hash(name);

			Components.emplace(name);

			ComponentFactory_Get.emplace(hash, [&](EntityID entity, entt::registry* registry) -> Component*
			{
				return &registry->get_or_emplace<T>((entt::entity)entity);
			});

			ComponentFactory_Add.emplace(hash, [&](EntityID entity, entt::registry* registry) -> Component*
			{
				return &registry->emplace<T>((entt::entity)entity);
			});

			ComponentFactory_Remove.emplace(hash, [&](EntityID entity, entt::registry* registry) -> size_t
			{
				return registry->remove<T>((entt::entity)entity);
			});

			ComponentFactory_Has.emplace(hash, [&](EntityID entity, entt::registry* registry) -> bool
			{
				return registry->try_get<T>((entt::entity)entity) != nullptr;
			});

		}


		static Component* GetComponent(const std::string& name, EntityID entity, entt::registry* registry);
		static Component* AddComponent(const std::string& name, EntityID entity, entt::registry* registry);
		static size_t RemoveComponent(const std::string& name, EntityID entity, entt::registry* registry);
		static bool HasComponent(const std::string& name, EntityID entity, entt::registry* registry);

		static std::unordered_set<std::string> GetComponentNames();

		template <ComponentType T>
		static bool IsComponentRegistered()
		{
			std::string name = T().Name();
			return Components.find(name) != Components.end();
		}

		template <ComponentType T>
		static bool IsType(Component* comp)
		{
			return Hash(comp->Name()) == Hash(T().Name());
		}


	private:
		static std::unordered_map<size_t, std::function<Component*(EntityID, entt::registry*)>> ComponentFactory_Get;
		static std::unordered_map<size_t, std::function<Component*(EntityID, entt::registry*)>> ComponentFactory_Add;
		static std::unordered_map<size_t, std::function<size_t(EntityID, entt::registry*)>> ComponentFactory_Remove;
		static std::unordered_map<size_t, std::function<bool(EntityID, entt::registry*)>> ComponentFactory_Has;

		static std::unordered_set<std::string> Components;
		
		static std::hash<std::string> Hash;

		
	};
}