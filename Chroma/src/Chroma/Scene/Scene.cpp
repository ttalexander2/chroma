#include "chromapch.h"

#include "Scene.h"
#include "Entity.h"
#include <Chroma/Components/Transform.h>
#include <Chroma/Components/Tag.h>
#include <Chroma/Components/AudioSource.h>
#include <Chroma/Components/SpriteRenderer.h>
#include <Chroma/Components/BoxCollider2D.h>
#include <Chroma/Components/CircleCollider2D.h>
#include <Chroma/Systems/AudioSystem.h>
#include <Chroma/Systems/SpriteRendererSystem.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>
#include <Chroma/Scene/System.h>
#include "Chroma/Utilities/GUID.h"
//#include <Chroma/Systems/ScriptingSystem.h>



namespace Chroma
{
	Scene::Scene()
	{
		ECS::RegisterComponent<Tag>();
		ECS::RegisterComponent<Transform>();
		ECS::RegisterComponent<AudioSource>();
		ECS::RegisterComponent<SpriteRenderer>();
		ECS::RegisterComponent<BoxCollider2D>();
		ECS::RegisterComponent<CircleCollider2D>();
		//RegisterSystem<ScriptingSystem>();
		RegisterSystem<AudioSystem>();
		RegisterSystem<SpriteRendererSystem>();
	}

	Entity Scene::NewEntity()
	{
		auto entity = Registry.create();
		Entity e = { entity, this };
		Registry.emplace<Tag>(entity).EntityName = "New Entity";
		Registry.emplace<Transform>(entity);

		return e;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		Registry.destroy(entity.GetID());
	}

	std::string Scene::Serialize()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << this->Name;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		auto view = Registry.view<Transform>();

		for (auto entity : view)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Entity" << YAML::Value << (uint32_t)entity;
			out << YAML::Key << "Components" << YAML::Value << YAML::BeginMap;

			for (auto& name : ECS::GetComponentNames())
			{
				if (ECS::HasComponent(name, entity, &Registry))
					ECS::GetComponent(name, entity, &Registry)->DoSerialize(out);
			}
			
			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::string result = out.c_str();
		return result;

	}

	bool Scene::Deserialize(Scene* out, const std::string& yaml)
	{
		uint64_t maxId = 0;
		auto data = YAML::Load(yaml);
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		CHROMA_CORE_TRACE("Deserializing Scene '{}'", sceneName);

		out->Name = sceneName;

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{

				EntityID id = (EntityID)entity["Entity"].as<uint32_t>();
				
				EntityID newEntity = out->Registry.create(id);

				CHROMA_CORE_TRACE("Deserialized Entity with ID = {0}", id);
				auto components = entity["Components"];
				if (components)
				{
					for (auto component : components)
					{
						std::string key = component.first.as<std::string>();
						ECS::AddComponent(key, newEntity, &out->Registry)->Deserialize(component.second);
					}
				}
			}
		}

		return true;



	}


	std::vector<Component*> Scene::GetAllComponents(EntityID entity)
	{
		std::vector<Component*> retval;
		for (std::string type : ECS::GetComponentNames())
		{
			if (ECS::HasComponent(type, entity, &Registry))
			{
				retval.push_back(ECS::GetComponent(type, entity, &Registry));
			}
		}

		return retval;
	}

	void Scene::EarlyInit()
	{
		for (System* s : Systems)
		{
			s->EarlyInit();
		}
	}

	void Scene::Init()
	{
		for (System* s : Systems)
		{
			s->Init();
		}
	}

	void Scene::LateInit()
	{
		for (System* s : Systems)
		{
			s->LateInit();
		}
	}

	void Scene::EarlyUpdate(Time delta)
	{
		for (System* s : Systems)
		{
			s->EarlyUpdate(delta);
		}
	}

	void Scene::Update(Time delta)
	{
		for (System* s : Systems)
		{
			s->Update(delta);
		}
	}

	void Scene::LateUpdate(Time delta)
	{
		for (System* s : Systems)
		{
			s->LateUpdate(delta);
		}
	}

	void Scene::PreDraw(Time delta)
	{
		for (System* s : Systems)
		{
			s->EarlyDraw(delta);
		}
	}

	void Scene::Draw(Time delta)
	{
		for (System* s : Systems)
		{
			s->Draw(delta);
		}
	}

	void Scene::PostDraw(Time delta)
	{
		for (System* s : Systems)
		{
			s->LateDraw(delta);
		}
	}

}