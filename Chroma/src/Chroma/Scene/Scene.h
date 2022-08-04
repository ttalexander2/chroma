#pragma once

#include <unordered_map>
#include <set>
#include <string>

#include "Chroma/Core/Core.h"
#include "Chroma/Core/Time.h"
#include "Component.h"

#include "Chroma/Core/Log.h"
#include "Chroma/Profiler/Instrumentor.h"
#include <Chroma/Components/Camera.h>
#include "World.h"
#include "Chroma/Components/Transform.h">


namespace Polychrome
{
	class Hierarchy;
	class Inspector;
}

namespace Chroma
{
	//Forward Declaration
	class Entity;
	class System;
	class PhysicsSystem;
	class ScriptingSystem;
	class AudioSystem;
	class SpriteRendererSystem;
	class ParticleSystem;
	class CameraSystem;


	//This entire class is a template-caused mess
	class Scene
	{
	public:
		std::string Name = "Scene";

		const GUID GetID();

		Scene();
		Scene(const Scene&) {}
		~Scene();

		Scene* Copy();

		Entity NewEntity();
		Entity NewChild(Entity id);

		std::string CreatePrefab(EntityID entity);
		void MakeUnique(EntityID entity);

		const Math::vec2 GetTransformAbsolutePosition(EntityID entity);
		void SetTransformAbsolutePosition(EntityID entity, const Math::vec2& position);

		void DestroyEntity(EntityID id, bool destroy_children = false);

		void SerializeEntity(YAML::Emitter& out, EntityID entity);

		std::string Serialize();
		static bool Deserialize(Scene* out, const std::string& yaml, bool load_assets = false);

		bool IsDescendant(EntityID child, EntityID parent);
		bool IsRoot(EntityID entity);
		EntityID GetRootEntity(EntityID child);
		std::vector<EntityID> FindAllDescendants(EntityID entity);

		std::vector<EntityID> GetChildren(EntityID entity);
		EntityID FindChildByName(EntityID entity, const std::string& child_name);
		EntityID GetFirstChild(EntityID entity);
		bool HasChildren(EntityID entity);
		size_t NumChildren(EntityID entity);

		Entity FindEntityByName(const std::string& name);

		template <ComponentType T>
		static void RegisterComponent()
		{
			T::CreateReflectionModel();
			StringHash hash = T::GetTypeInfoStatic()->GetType();
			if (s_ComponentAdd.find(hash) != s_ComponentAdd.end())
			{
				CHROMA_CORE_WARN("Component [{}] already registered!", T::GetTypeNameStatic());
				return;
			}
			const TypeInfo* type = T::GetTypeInfoStatic();
			s_Types.push_back(type);

			s_ComponentAdd[hash] = [](EntityID id, entt::registry* registry)
			{
				if (T::GetTypeNameStatic() == "ParticleEmitter")
				{
					CHROMA_CORE_INFO("added particle emitter");
				}
				registry->emplace<T>(id, id);
				return registry->try_get<T>(id);
			};

			s_ComponentHas[hash] = [](EntityID id, entt::registry* registry)
			{
				return registry->try_get<T>(id) != nullptr;
			};

			s_ComponentGet[hash] = [](EntityID id, entt::registry* registry)
			{
				return registry->try_get<T>(id);
			};

			s_ComponentRemove[hash] = [](EntityID id, entt::registry* registry)
			{
				registry->erase<T>(id);
			};

			s_CopyComponent[hash] = [](EntityID id, entt::registry* dst, entt::registry* src)
			{
				dst->emplace<T>(id, src->get<T>(id));
			};
		}

		template <ComponentType T>
		static bool IsComponentRegistered()
		{
			StringHash hash = T::GetTypeInfoStatic()->GetType();
			return s_ComponentAdd.find(hash) != s_ComponentAdd.end();
		}

		static bool IsComponentRegistered(const std::string& component_name)
		{
			StringHash hash = StringHash::Hash(component_name.c_str());
			return s_ComponentAdd.contains(hash);

		}

		static std::list<const TypeInfo*> GetComponentTypes();


		template<ComponentType T>
		T& AddComponent(EntityID id)
		{
			auto& comp = Registry.emplace<T>(id, id);
			return comp;
		}

		Component* AddComponent(const std::string& component, EntityID entity)
		{
			StringHash hash = StringHash::Hash(component.c_str());
			if (!s_ComponentAdd.contains(hash))
				return nullptr;
			return s_ComponentAdd[hash](entity, &Registry);
		}

		template<ComponentType T>
		T& GetComponent(EntityID id)
		{
			return Registry.get<T>(id);
		}

		Component* GetComponent(const std::string& component, EntityID entity)
		{
			StringHash hash = StringHash::Hash(component.c_str());
			if (!s_ComponentGet.contains(hash))
				return nullptr;
			return s_ComponentGet[hash](entity, &Registry);
		}

		template<ComponentType T>
		bool HasComponent(EntityID id)
		{
			return Registry.try_get<T>(id) != nullptr;
		}

		bool HasComponent(const std::string& component, EntityID entity)
		{
			StringHash hash = StringHash::Hash(component.c_str());
			if (!s_ComponentHas.contains(hash))
				return false;
			return s_ComponentHas[hash](entity, &Registry);
		}

		/*
		template<ComponentType T>
		void RemoveComponents(EntityID id)
		{
			//Recursively check
		}

		template<ComponentType T>
		void RemoveComponents(Entity id)
		{
			RemoveComponents<T>((EntityID)id);
		}
		*/

		template<ComponentType T>
		size_t RemoveComponent(EntityID entity)
		{
			return Registry.remove<T>(entity);
		}

		size_t RemoveComponent(const std::string& component, EntityID entity)
		{
			StringHash hash = StringHash::Hash(component.c_str());
			if (!s_ComponentHas.contains(hash))
				return false;
			s_ComponentRemove[hash](entity, &Registry);
			return true;
		}

		std::vector<Component*> GetAllComponents(EntityID entity);




		//template<ComponentType... Types>
		//auto View() { return Registry.view<Types>(); }

#pragma region UpdateLoop

		//TODO: Load & Unload Functions

		void OnLoad();


		void Init();
		void Update(Time delta);
		void Draw(Time delta);


		Camera& GetPrimaryCamera();
		EntityID GetPrimaryCameraEntity() { return PrimaryCameraEntity; }
		bool SetPrimaryCamera(EntityID entity);

		std::vector<Collider*> GetColliders(EntityID entity);

#pragma endregion
		entt::registry Registry;

		std::unordered_map<size_t, EntityID> Tags;
		//std::unordered_map<unsigned int, EntityID> Layers;


		std::vector<Layer> Layers;

	private:
		
		PhysicsSystem* physics_system;
		ScriptingSystem* scripting_system;
		AudioSystem* audio_system;
		SpriteRendererSystem* sprite_renderer_system;
		ParticleSystem* particle_system;
		CameraSystem* camera_system;


		std::vector<EntityID> EntityOrder;
		EntityID PrimaryCameraEntity = ENTITY_NULL;

		GUID ID;

		static std::unordered_map<StringHash, std::function<Component* (EntityID, entt::registry*)>> s_ComponentAdd;
		static std::unordered_map<StringHash, std::function<Component* (EntityID, entt::registry*)>> s_ComponentGet;
		static std::unordered_map<StringHash, std::function<bool(EntityID, entt::registry*)>> s_ComponentHas;
		static std::unordered_map<StringHash, std::function<void(EntityID, entt::registry*)>> s_ComponentRemove;
		static std::unordered_map<StringHash, std::function<void(EntityID, entt::registry*, entt::registry*)>> s_CopyComponent;

		static std::list<const TypeInfo*> s_Types;


		friend class Entity;
		friend class MonoScripting;
		friend class Polychrome::Hierarchy;
		friend class Polychrome::Inspector;


	};

}