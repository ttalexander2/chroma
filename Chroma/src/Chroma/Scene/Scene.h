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
#include "Chroma/Components/Transform.h"
#include "ComponentRegistry.h"


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

		Scene(const Scene &)
		{
		}

		~Scene();

		Scene *Copy();

		Entity NewEntity();
		Entity NewChild(Entity id);

		std::string CreatePrefab(EntityID entity);
		void MakeUnique(EntityID entity);

		const Math::vec2 GetTransformAbsolutePosition(EntityID entity);
		void SetTransformAbsolutePosition(EntityID entity, const Math::vec2 &position);

		void DestroyEntity(EntityID id, bool destroy_children = false);

		void SerializeEntity(YAML::Emitter &out, EntityID entity);
		static void DeserializeEntity(EntityID id, YAML::Node &node, Scene *out);

		std::string Serialize();
		static bool Deserialize(Scene *out, const std::string &yaml, bool load_assets = false);

		bool IsDescendant(EntityID child, EntityID parent);
		bool IsRoot(EntityID entity);
		EntityID GetRootEntity(EntityID child);
		std::vector<EntityID> FindAllDescendants(EntityID entity);

		std::vector<EntityID> GetChildren(EntityID entity);
		EntityID FindChildByName(EntityID entity, const std::string &child_name);
		EntityID GetFirstChild(EntityID entity);
		bool HasChildren(EntityID entity);
		size_t NumChildren(EntityID entity);

		Entity FindEntityByName(const std::string &name);


		template <ComponentType T>
		T &AddComponent(EntityID id)
		{
			return static_cast<T &>(*ComponentRegistry::AddComponent(Reflection::resolve<T>().id(), id, &Registry));
		}

		Component *AddComponent(const std::string &component, EntityID entity)
		{
			return ComponentRegistry::AddComponent(component, entity, &Registry);
		}

		template <ComponentType T>
		T &GetComponent(EntityID id)
		{
			T *comp = Registry.try_get<T>(id);
			if (comp == nullptr)
				comp = &Registry.emplace<T>(id);
			return *comp;
		}

		Component *GetComponent(const std::string &component, EntityID entity)
		{
			return ComponentRegistry::GetComponent(component, entity, &Registry);
		}

		Component *GetComponent(uint32_t component_id, EntityID entity)
		{
			return ComponentRegistry::GetComponent(component_id, entity, &Registry);
		}

		static std::vector<Reflection::type> GetComponentTypes();

		template <ComponentType T>
		bool HasComponent(EntityID id)
		{
			return Registry.try_get<T>(id) != nullptr;
		}

		bool HasComponent(const std::string &component, EntityID entity)
		{
			return ComponentRegistry::HasComponent(component, entity, &Registry);
		}

		template <ComponentType T>
		size_t RemoveComponent(EntityID entity)
		{
			return Registry.remove<T>(entity);
		}

		size_t RemoveComponent(const std::string &component, EntityID entity)
		{
			return ComponentRegistry::RemoveComponent(component, entity, &Registry);
		}

		std::vector<Component *> GetAllComponents(EntityID entity);


		void OnLoad();


		void Init();
		void Update(Time delta);
		void Draw(Time delta);


		Camera &GetPrimaryCamera();
		EntityID GetPrimaryCameraEntity() { return PrimaryCameraEntity; }
		bool SetPrimaryCamera(EntityID entity);

		std::vector<Collider *> GetColliders(EntityID entity);

		entt::registry Registry;

		std::unordered_map<size_t, EntityID> Tags;
		//std::unordered_map<unsigned int, EntityID> Layers;


		std::vector<Layer> Layers;

	private:
		PhysicsSystem *physics_system;
		ScriptingSystem *scripting_system;
		AudioSystem *audio_system;
		SpriteRendererSystem *sprite_renderer_system;
		ParticleSystem *particle_system;
		CameraSystem *camera_system;

		std::vector<EntityID> EntityOrder;
		EntityID PrimaryCameraEntity = ENTITY_NULL;

		GUID ID;

		friend class Entity;
		friend class MonoScripting;
		friend class Polychrome::Hierarchy;
		friend class Polychrome::Inspector;
	};
}
