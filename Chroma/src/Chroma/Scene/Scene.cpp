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
#include <Chroma/Systems/ScriptingSystem.h>
#include <Chroma/Components/Relationship.h>
#include <Chroma/Utilities/ContainerHelpers.h>
//#include <Chroma/Systems/ScriptingSystem.h>



namespace Chroma
{
	Scene::Scene()
	{
		RegisterSystem<ScriptingSystem>();
		RegisterSystem<AudioSystem>();
		RegisterSystem<SpriteRendererSystem>();
	}

	Entity Scene::NewEntity()
	{
		auto entity = Registry.create();
		Entity e = { entity, this };
		Registry.emplace<Tag>(entity).EntityName = fmt::format("Entity_{}", entity);
		Registry.emplace<Transform>(entity);
		Registry.emplace<Relationship>(entity);

		EntityOrder.push_back(entity);

		return e;
	}

	Entity Scene::NewChild(Entity parent)
	{
		auto entity = Registry.create();
		Entity e = { entity, this };
		Registry.emplace<Tag>(entity).EntityName = fmt::format("Entity_{}", entity);
		Registry.emplace<Transform>(entity);
		Registry.emplace<Relationship>(entity);

		auto& child_r = e.GetComponent<Relationship>();
		auto& parent_r = GetComponent<Relationship>((EntityID)parent);
		parent_r.Children.push_back(e.GetID());
		child_r.Parent = (EntityID)parent;
		return e;
		
	}

	void Scene::DestroyEntity(EntityID entity, bool destroy_children)
	{
		auto& rel = Registry.get<Relationship>(entity);
		if (destroy_children)
		{
			for (EntityID child : FindAllDescendants(entity))
			{
				Registry.destroy(child);
			}
		}
		else
		{
			for (EntityID child : rel.Children)
			{
				auto& crel = Registry.get<Relationship>(child);
				crel.Parent = rel.Parent;
				if (rel.Parent == ENTITY_NULL)
					EntityOrder.push_back(child);
				else
					Registry.get<Relationship>(rel.Parent).Children.push_back(child);
			}
		}

		if (rel.Parent == ENTITY_NULL)
		{
			PopValue(EntityOrder, entity);
		}
		else
		{
			PopValue(Registry.get<Relationship>(rel.Parent).Children, entity);
		}
		Registry.destroy(entity);
	}

	std::vector<EntityID> Scene::FindAllDescendants(EntityID entity)
	{
		auto& rel = Registry.get<Relationship>(entity);
		std::vector<EntityID> desc;
		for (auto child : rel.Children)
		{
			desc.push_back(child);
			auto results = FindAllDescendants(child);
			desc.insert(desc.end(), results.begin(), results.end());
		}
		return desc;
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
				{
					ECS::GetComponent(name, entity, &Registry)->DoSerialize(out);
				}
					
			}
			
			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;

		out << YAML::Key << "RootEntities" << YAML::Value << YAML::Flow << YAML::BeginSeq;

		for (auto& e : EntityOrder)
		{
			out << (uint32_t)e;
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

				CHROMA_CORE_TRACE("Deserialized Entity with ID = {0} (hint={1})", newEntity, id);
				auto components = entity["Components"];
				if (components)
				{
					for (auto component : components)
					{
						std::string key = component.first.as<std::string>();
						if (key == "LuaScript")
							ECS::AddComponent(key, newEntity, &out->Registry)->Deserialize(component.second, newEntity, out);
						else
							ECS::AddComponent(key, newEntity, &out->Registry)->Deserialize(component.second);
					}
				}
			}
		}

		auto order = data["RootEntities"];
		if (order)
		{
			for (auto id : order)
			{
				out->EntityOrder.push_back((EntityID)id.as<uint32_t>());
			}
		}

		return true;



	}

	bool Scene::IsDescendant(EntityID child, EntityID parent)
	{

		if (parent == Chroma::ENTITY_NULL || child == Chroma::ENTITY_NULL)
			return false;

		auto& p_rel = GetComponent<Chroma::Relationship>(parent);
		auto& c_rel = GetComponent<Chroma::Relationship>(child);

		auto& cp = child;

		while (cp != Chroma::ENTITY_NULL)
		{
			auto rpp = GetComponent<Chroma::Relationship>(cp).Parent;
			if (rpp == parent)
				return true;
			cp = rpp;
		}
		return false;

	}

	bool Scene::IsRoot(EntityID entity)
	{
		if (entity == ENTITY_NULL)
			return false;
		auto& r = GetComponent<Chroma::Relationship>(entity);
		return r.Parent == ENTITY_NULL;
	}

	EntityID Scene::GetRootEntity(EntityID child)
	{
		if (child == ENTITY_NULL)
			return ENTITY_NULL;

		auto& c_rel = GetComponent<Chroma::Relationship>(child);

		auto& cp = child;

		while (cp != Chroma::ENTITY_NULL)
		{
			auto rpp = GetComponent<Chroma::Relationship>(cp).Parent;
			if (rpp == ENTITY_NULL)
				return cp;
			cp = rpp;
		}
		return ENTITY_NULL;
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

	void Scene::PreLoad()
	{
		for (System* s : Systems)
		{
			s->PreLoad();
		}
	}

	void Scene::Load()
	{
		for (System* s : Systems)
		{
			s->Load();
		}
	}

	void Scene::PostLoad()
	{
		for (System* s : Systems)
		{
			s->PostLoad();
		}
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