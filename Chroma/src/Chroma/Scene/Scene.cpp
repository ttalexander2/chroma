#include "chromapch.h"

#include "Scene.h"
#include "Entity.h"
#include <Chroma/Components/Transform.h>
#include <Chroma/Components/Tag.h>
#include <Chroma/Components/AudioSource.h>
#include <Chroma/Components/SpriteRenderer.h>
#include <Chroma/Components/BoxCollider.h>
#include <Chroma/Components/CircleCollider.h>
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
#include <Chroma/Systems/CameraSystem.h>
#include <Chroma/Systems/ParticleSystem.h>
#include <Chroma/Components/CSharpScript.h>



namespace Chroma
{

	std::unordered_map<StringHash, std::function<Component* (EntityID, entt::registry*)>> Scene::s_ComponentAdd;
	std::unordered_map<StringHash, std::function<Component* (EntityID, entt::registry*)>> Scene::s_ComponentGet;
	std::unordered_map<StringHash, std::function<bool(EntityID, entt::registry*)>> Scene::s_ComponentHas;
	std::unordered_map<StringHash, std::function<void(EntityID, entt::registry*)>> Scene::s_ComponentRemove;
	std::unordered_map<StringHash, std::function<void(EntityID, entt::registry*, entt::registry*)>> Scene::s_CopyComponent;

	std::list<const TypeInfo*> Scene::s_Types;

	Scene::Scene()
		: ID(GUID::CreateGUID())
	{
		RegisterSystem<ScriptingSystem>();
		RegisterSystem<AudioSystem>();
		RegisterSystem<SpriteRendererSystem>();
		RegisterSystem<ParticleSystem>();
		RegisterSystem<CollisionSystem>();
		RegisterSystem<CameraSystem>();

	}

	const GUID Scene::GetID() { return ID; }

	Scene* Scene::Copy()
	{
		Scene* out = new Scene();
		out->EntityOrder = std::vector<Chroma::EntityID>(EntityOrder);
		out->ID = ID;
		out->Layers = std::vector<Chroma::Layer>(Layers);
		out->PrimaryCameraEntity = PrimaryCameraEntity;
		for (auto oldEntity : Registry.view<Chroma::Transform>())
		{
			auto newEntity = out->Registry.create(oldEntity);
			for (Component* c : GetAllComponents(oldEntity))
			{
				s_CopyComponent[c->GetType()](newEntity, &out->Registry, &Registry);
			}
		}
		return out;
	}

	Entity Scene::NewEntity()
	{
		auto entity = Registry.create();
		Entity e = { entity, this };
		Registry.emplace<Tag>(entity, entity).EntityName = fmt::format("Entity_{}", entity);
		Registry.emplace<Transform>(entity, entity);
		Registry.emplace<Relationship>(entity, entity);

		EntityOrder.push_back(entity);
		return e;
	}

	Entity Scene::NewChild(Entity parent)
	{
		auto entity = Registry.create();
		Entity e = { entity, this };
		Registry.emplace<Tag>(entity, entity).EntityName = fmt::format("Entity_{}", entity);
		Registry.emplace<Transform>(entity, entity);
		Registry.emplace<Relationship>(entity, entity);

		auto& child_r = e.GetComponent<Relationship>();
		auto& parent_r = GetComponent<Relationship>((EntityID)parent);
		parent_r.Children.push_back(e.GetID());
		child_r.Parent = (EntityID)parent;
		return e;
		
	}

	std::string Scene::CreatePrefab(EntityID entity)
	{
		
		YAML::Emitter out;
		SerializeEntity(out, entity);
		return std::string(out.c_str());
	}

	void Scene::MakeUnique(EntityID entity)
	{
	}

	const Math::vec2 Scene::GetTransformAbsolutePosition(EntityID entity)
	{
		Transform& transform = Registry.get<Transform>(entity);
		Relationship& relationship = Registry.get<Relationship>(entity);
		if (!relationship.IsChild())
			return transform.Position;
		Math::vec2 finalPos = transform.Position;
		auto parentID = relationship.Parent;
		while (parentID != ENTITY_NULL)
		{
			Transform& parentTransform = Registry.get<Transform>(parentID);
			Relationship& ParentRelationship = Registry.get<Relationship>(parentID);
			finalPos += parentTransform.Position;
			parentID = ParentRelationship.Parent;
		}
		return finalPos;
	}

	void Scene::SetTransformAbsolutePosition(EntityID entity, const Math::vec2& position)
	{
		Transform& transform = Registry.get<Transform>(entity);
		Relationship& relationship = Registry.get<Relationship>(entity);
		if (!relationship.IsChild())
		{
			transform.Position = { position.x, position.y };
			return;
		}
		Math::vec2 finalPos = position;
		auto parentID = relationship.Parent;
		while (parentID != ENTITY_NULL)
		{
			Transform& parentTransform = Registry.get<Transform>(parentID);
			Relationship& ParentRelationship = Registry.get<Relationship>(parentID);
			finalPos -= parentTransform.Position;
			parentID = ParentRelationship.Parent;
		}
		transform.Position = finalPos;
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

	std::vector<EntityID> Scene::GetChildren(EntityID entity)
	{
		std::vector<EntityID> result;

		if (!Registry.valid(entity))
		{
			CHROMA_CORE_ERROR("Entity [{}] not valid!", entity);
			return result;
		}

		auto& rel = Registry.get<Relationship>(entity);
		for (auto child : rel.Children)
		{
			result.push_back(child);
		}

		return result;
	}

	EntityID Scene::FindChildByName(EntityID entity, const std::string& child_name)
	{

		if (!Registry.valid(entity))
		{
			CHROMA_CORE_ERROR("Entity [{}] not valid!", entity);
			return Chroma::ENTITY_NULL;
		}

		auto& rel = Registry.get<Relationship>(entity);
		for (auto child : rel.Children)
		{
			auto& tag = Registry.get<Tag>(entity);
			if (tag.EntityName == child_name)
				return child;
		}

		return Chroma::ENTITY_NULL;
	}

	EntityID Scene::GetFirstChild(EntityID entity)
	{
		if (!Registry.valid(entity))
		{
			CHROMA_CORE_ERROR("Entity [{}] not valid!", entity);
			return Chroma::ENTITY_NULL;
		}

		auto& rel = Registry.get<Relationship>(entity);
		if (!rel.HasChildren() || rel.Children.size() <= 0)
		{
			CHROMA_CORE_ERROR("Entity [{}] does not have children!", entity);
			return Chroma::ENTITY_NULL;
		}

		return rel.Children[0];
	}

	bool Scene::HasChildren(EntityID entity)
	{
		if (!Registry.valid(entity))
		{
			CHROMA_CORE_ERROR("Entity [{}] not valid!", entity);
			return false;
		}

		auto& rel = Registry.get<Relationship>(entity);
		return rel.HasChildren();
	}

	size_t Scene::NumChildren(EntityID entity)
	{
		if (!Registry.valid(entity))
		{
			CHROMA_CORE_ERROR("Entity [{}] not valid!", entity);
			return 0;
		}

		auto& rel = Registry.get<Relationship>(entity);
		return rel.Children.size();
	}

	Entity Scene::FindEntityByName(const std::string& name)
	{
		for (auto entity : Registry.view<Chroma::Tag>())
		{
			Chroma::Tag& tag = Registry.get<Chroma::Tag>(entity);
			if (name == tag.EntityName)
			{
				return Entity(entity, this);
			}
		}
	}

	std::list<const TypeInfo*> Scene::GetComponentTypes()
	{
		return Scene::s_Types;
	}

	std::string Scene::Serialize()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << this->Name;
		out << YAML::Key << "GUID" << YAML::Value << this->ID.ToString();
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		auto view = Registry.view<Transform>();

		for (auto entity : view)
		{
			SerializeEntity(out, entity);
		}

		out << YAML::EndSeq;

		out << YAML::Key << "RootEntities" << YAML::Value << YAML::Flow << YAML::BeginSeq;

		for (auto& e : EntityOrder)
		{
			out << (uint32_t)e;
		}

		out << YAML::EndSeq;

		out << YAML::Key << "Layers" << YAML::Value << YAML::BeginSeq;
		for (auto& layer : Layers)
		{
			//CHROMA_CORE_TRACE("layer: {}", layer.Name);
			out << YAML::BeginMap;
			layer.Serialize(out);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;

		out << YAML::EndMap;

		std::string result = out.c_str();

		return result;

	}

	bool Scene::Deserialize(Scene* out, const std::string& yaml, bool load_assets)
	{
		uint64_t maxId = 0;
		auto data = YAML::Load(yaml);
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		//CHROMA_CORE_TRACE("Deserializing Scene '{}'", sceneName);

		std::string guid_string = data["GUID"].as<std::string>();
		GUID guid = GUID::Parse(guid_string);

		if (guid == GUID::Zero())
			guid = GUID::CreateGUID();

		out->ID = guid;

		out->Name = sceneName;

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{

				EntityID id = (EntityID)entity["Entity"].as<uint32_t>();
				
				EntityID newEntity = out->Registry.create(id);

				auto name = entity["Name"];
				Tag& tag = out->Registry.get_or_emplace<Tag>(newEntity);
				if (name)
					tag.EntityName = name.as<std::string>();

				//CHROMA_CORE_TRACE("Deserialized Entity with ID = {0} (hint={1})", newEntity, id);
				auto components = entity["Components"];
				if (components)
				{
					int i = 1;
					for (auto component : components)
					{
						std::string key = component.first.as<std::string>();

						if (!Scene::IsComponentRegistered(key))
						{
							CHROMA_CORE_ERROR("Unable to construct component of type [{}]. No registered component found!", key);
							continue;
						}

						auto created = out->AddComponent(key, newEntity);
						if (key == "Transform" || key == "Relationship")
							created->order_id = 0;
						else
							created->order_id = i;

						created->Deserialize(component.second);
						if (key == Camera::GetTypeNameStatic())
						{
							auto val = component.second["Primary"];
							if (val)
							{
								bool primary = val.as<bool>();
								if (primary)
									out->SetPrimaryCamera(newEntity);
							}
						}

						i++;
					}

					out->Registry.get_or_emplace<Relationship>(newEntity, newEntity);
				}
			}
			auto view = out->Registry.view<Camera>();
			if (view.size() == 0)
			{
				auto newCam = out->NewEntity();
				out->AddComponent<Camera>(newCam.GetID());
				out->PrimaryCameraEntity = newCam.GetID();
				out->GetComponent<Tag>(newCam.GetID()).EntityName = "Camera";
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

		out->Layers.clear();
		auto layers = data["Layers"];
		if (layers)
		{
			for (auto layer : layers)
			{
				Layer l = Layer("", 0);
				l.Deserialize(layer);
				out->Layers.push_back(l);
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
		for (auto& [type, func] : s_ComponentGet)
		{
			Component* comp = func(entity, &Registry);
			if (comp != nullptr)
				retval.push_back(comp);
		}

		return retval;
	}

	void Scene::SerializeEntity(YAML::Emitter& out, EntityID entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << (uint32_t)entity;

		Tag& tag = Registry.get_or_emplace<Tag>(entity);
		out << YAML::Key << "Name" << YAML::Value << tag.EntityName;

		out << YAML::Key << "Components" << YAML::Value << YAML::BeginMap;

		std::vector<Component*> components = this->GetAllComponents(entity);
		std::sort(components.begin(), components.end(), [](Component* a, Component* b) {return a->order_id < b->order_id; });

		for (Component* comp : components)
		{
			if (comp->IsTypeOf<Relationship>())
			{
				Relationship* rel = reinterpret_cast<Relationship*>(comp);
				if (rel->HasChildren() || rel->IsChild())
					comp->DoSerialize(out);

			}
			else if (comp->IsTypeOf<Tag>())
			{
				continue;
			}
			else if (comp->IsTypeOf<Camera>())
			{
				comp->BeginSerialize(out);
				comp->Serialize(out);
				if (PrimaryCameraEntity == entity)
				{
					out << YAML::Key << "Primary";
					out << YAML::Value << true;
				}
				comp->EndSerialize(out);

			}
			else
			{
				comp->DoSerialize(out);
			}

		}

		out << YAML::EndMap;
		out << YAML::EndMap;
	}

	void Scene::OnLoad()
	{
		for (System* s : Systems)
		{
			s->PreLoad();
		}

		for (System* s : Systems)
		{
			s->Load();
		}

		for (System* s : Systems)
		{
			s->PostLoad();
		}
	}

	void Scene::Init()
	{
		for (System* s : Systems)
		{
			s->EarlyInit();
		}

		for (System* s : Systems)
		{
			s->Init();
		}

		for (System* s : Systems)
		{
			s->LateInit();
		}
	}

	void Scene::Update(Time delta)
	{
		for (System* s : Systems)
		{
			s->EarlyUpdate(delta);
		}

		for (System* s : Systems)
		{
			s->Update(delta);
		}

		for (System* s : Systems)
		{
			s->LateUpdate(delta);
		}
	}

	void Scene::Draw(Time delta)
	{
		for (System* s : Systems)
		{
			s->EarlyDraw(delta);
		}

		for (System* s : Systems)
		{
			s->Draw(delta);
		}

		for (System* s : Systems)
		{
			s->LateDraw(delta);
		}
	}

	Camera& Scene::GetPrimaryCamera()
	{
		if (PrimaryCameraEntity == ENTITY_NULL)
		{
			auto ent = NewEntity();
			return ent.AddComponent<Camera>();
		}
		return Registry.get_or_emplace<Camera>(PrimaryCameraEntity, PrimaryCameraEntity);
	}

	bool Scene::SetPrimaryCamera(EntityID entity)
	{
		if (!Registry.valid(entity))
		{
			CHROMA_CORE_ERROR("Invalid entity '{}'", entity);
			return false;
		}

		PrimaryCameraEntity = entity;
		Registry.get_or_emplace<Camera>(PrimaryCameraEntity, PrimaryCameraEntity);
		return true;
	}

}