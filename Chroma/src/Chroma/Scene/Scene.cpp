#include "chromapch.h"

#include "Scene.h"
#include "Entity.h"
#include <Chroma/Components/Transform.h>
#include <Chroma/Components/Tag.h>
#include <Chroma/Components/AudioSource.h>
#include <Chroma/Components/SpriteRenderer.h>


#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>
#include <Chroma/Scene/System.h>
#include "Chroma/Utilities/GUID.h"

#include <Chroma/Utilities/ContainerHelpers.h>
#include <Chroma/Components/CSharpScript.h>

#include <Chroma/Systems/AudioSystem.h>
#include <Chroma/Systems/SpriteRendererSystem.h>
#include <Chroma/Systems/ScriptingSystem.h>
#include <Chroma/Systems/CameraSystem.h>
#include <Chroma/Systems/ParticleSystem.h>
#include <Chroma/Systems/PhysicsSystem.h>
#include <Chroma/Components/CircleCollider.h>
#include <Chroma/Components/EdgeCollider.h>
#include <Chroma/Components/PolygonCollider.h>
#include <Chroma/Components/RectangleCollider.h>
#include <Chroma/Reflection/Reflection.h>





namespace Chroma
{

	Scene::Scene()
		: ID(GUID::CreateGUID())
	{

		physics_system = new PhysicsSystem();
		scripting_system = new ScriptingSystem();
		audio_system = new AudioSystem();
		sprite_renderer_system = new SpriteRendererSystem();
		particle_system = new ParticleSystem();
		camera_system = new CameraSystem();

		physics_system->m_Scene = this;
		scripting_system->m_Scene = this;
		audio_system->m_Scene = this;
		sprite_renderer_system->m_Scene = this;
		particle_system->m_Scene = this;
		camera_system->m_Scene = this;
	}

	Scene::~Scene()
	{
		delete physics_system;
		delete scripting_system;
		delete audio_system;
		delete sprite_renderer_system;
		delete particle_system;
		delete camera_system;
	}

	const GUID Scene::GetID() { return ID; }

	Scene* Scene::Copy()
	{
		Scene* out = new Scene();
		out->EntityOrder = std::vector<Chroma::EntityID>(EntityOrder);
		out->ID = ID;
		out->Layers = std::vector<Chroma::Layer>(Layers);
		for (auto oldEntity : Registry.view<Chroma::Transform>())
		{
			auto newEntity = out->Registry.create(oldEntity);
			for (Component* c : GetAllComponents(oldEntity))
			{
				ComponentRegistry::ComponentCopyFunctions()[c->TypeId()](newEntity, &out->Registry, &Registry);
			}
			if (oldEntity == PrimaryCameraEntity)
			{
				out->PrimaryCameraEntity = newEntity;
			}
		}
		return out;
	}

	Entity Scene::NewEntity()
	{
		const auto entity = Registry.create();
		Entity e = { entity, this };
		Registry.emplace<Tag>(entity, entity).EntityName = fmt::format("Entity_{}", entity);
		Registry.emplace<Transform>(entity, entity);

		EntityOrder.push_back(entity);
		return e;
	}

	Entity Scene::NewChild(Entity parent)
	{
		auto entity = Registry.create();
		Entity e = { entity, this };
		Registry.emplace<Tag>(entity, entity).EntityName = fmt::format("Entity_{}", entity);
		Registry.emplace<Transform>(entity, entity);

		auto& child_r = e.GetComponent<Transform>();
		auto& parent_r = GetComponent<Transform>((EntityID)parent);
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
		if (!transform.IsChild())
			return transform.Position;
		Math::vec2 finalPos = transform.Position;
		auto parentID = transform.Parent;
		while (parentID != ENTITY_NULL)
		{
			Transform& parentTransform = Registry.get<Transform>(parentID);
			finalPos += parentTransform.Position;
			parentID = parentTransform.Parent;
		}
		return finalPos;
	}

	void Scene::SetTransformAbsolutePosition(EntityID entity, const Math::vec2& position)
	{
		Transform& transform = Registry.get<Transform>(entity);
		if (!transform.IsChild())
		{
			transform.Position = Math::vec2(position.x, position.y);
			return;
		}
		Math::vec2 finalPos = position;
		auto parentID = transform.Parent;
		while (parentID != ENTITY_NULL)
		{
			Transform& parentTransform = Registry.get<Transform>(parentID);
			finalPos -= parentTransform.Position;
			parentID = parentTransform.Parent;
		}
		transform.Position = finalPos;
	}

	void Scene::DestroyEntity(EntityID entity, bool destroy_children)
	{
		auto& rel = Registry.get<Transform>(entity);
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
				auto& crel = Registry.get<Transform>(child);
				crel.Parent = rel.Parent;
				if (rel.Parent == ENTITY_NULL)
					EntityOrder.push_back(child);
				else
					Registry.get<Transform>(rel.Parent).Children.push_back(child);
			}
		}

		if (rel.Parent == ENTITY_NULL)
		{
			PopValue(EntityOrder, entity);
		}
		else
		{
			PopValue(Registry.get<Transform>(rel.Parent).Children, entity);
		}
		Registry.destroy(entity);
	}

	std::vector<EntityID> Scene::FindAllDescendants(EntityID entity)
	{
		auto &rel = Registry.get<Transform>(entity);
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

		auto &rel = Registry.get<Transform>(entity);
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

		auto &rel = Registry.get<Transform>(entity);
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

		auto &rel = Registry.get<Transform>(entity);
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

		auto &rel = Registry.get<Transform>(entity);
		return rel.HasChildren();
	}

	size_t Scene::NumChildren(EntityID entity)
	{
		if (!Registry.valid(entity))
		{
			CHROMA_CORE_ERROR("Entity [{}] not valid!", entity);
			return 0;
		}

		auto &rel = Registry.get<Transform>(entity);
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

	std::string Scene::Serialize()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << this->Name;
		out << YAML::Key << "GUID" << YAML::Value << this->ID.ToString();

		out << YAML::Key << "Primary Camera";
		out << YAML::Value << PrimaryCameraEntity;

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
		//uint64_t maxId = 0;
		//auto data = YAML::Load(yaml);
		//if (!data["Scene"])
		//	return false;
		//
		//std::string sceneName = data["Scene"].as<std::string>();
		////CHROMA_CORE_TRACE("Deserializing Scene '{}'", sceneName);
		//
		//std::string guid_string = data["GUID"].as<std::string>();
		//GUID guid = GUID::Parse(guid_string);
		//
		//if (guid == GUID::Zero())
		//	guid = GUID::CreateGUID();
		//
		//out->ID = guid;
		//
		//out->Name = sceneName;
		//
		//auto entities = data["Entities"];
		//if (entities)
		//{
		//	for (auto entity : entities)
		//	{
		//
		//		EntityID id = (EntityID)entity["Entity"].as<uint32_t>();
		//		
		//		EntityID newEntity = out->Registry.create(id);
		//
		//		auto name = entity["Name"];
		//		Tag& tag = out->Registry.get_or_emplace<Tag>(newEntity);
		//		if (name)
		//			tag.EntityName = name.as<std::string>();
		//
		//		//CHROMA_CORE_TRACE("Deserialized Entity with ID = {0} (hint={1})", newEntity, id);
		//		auto components = entity["Components"];
		//		if (components)
		//		{
		//			int i = 1;
		//			for (auto component : components)
		//			{
		//				std::string key = component.first.as<std::string>();
		//
		//				if (!Scene::IsComponentRegistered(key))
		//				{
		//					CHROMA_CORE_ERROR("Unable to construct component of type [{}]. No registered component found!", key);
		//					continue;
		//				}
		//
		//				auto created = out->AddComponent(key, newEntity);
		//				if (key == "Transform")
		//					created->order_id = 0;
		//				else
		//					created->order_id = i;
		//
		//				created->DoDeserialize(component.second);
		//				if (key == Reflect::Resolve<Camera>()->GetName())
		//				{
		//					auto val = component.second["Primary"];
		//					if (val)
		//					{
		//						bool primary = val.as<bool>();
		//						if (primary)
		//							out->SetPrimaryCamera(newEntity);
		//					}
		//				}
		//
		//				i++;
		//			}
		//
		//			out->Registry.get_or_emplace<Transform>(newEntity, newEntity);
		//		}
		//	}
		//	auto view = out->Registry.view<Camera>();
		//	if (view.size() == 0)
		//	{
		//		auto newCam = out->NewEntity();
		//		out->AddComponent<Camera>(newCam.GetID());
		//		out->PrimaryCameraEntity = newCam.GetID();
		//		out->GetComponent<Tag>(newCam.GetID()).EntityName = "Camera";
		//	}
		//}
		//
		//auto order = data["RootEntities"];
		//if (order)
		//{
		//	for (auto id : order)
		//	{
		//		out->EntityOrder.push_back((EntityID)id.as<uint32_t>());
		//	}
		//}
		//
		//out->Layers.clear();
		//auto layers = data["Layers"];
		//if (layers)
		//{
		//	for (auto layer : layers)
		//	{
		//		Layer l = Layer("", 0);
		//		l.Deserialize(layer);
		//		out->Layers.push_back(l);
		//	}
		//}
		//


		//Remove this
		auto newCam = out->NewEntity();
		out->AddComponent<Camera>(newCam.GetID());
		out->PrimaryCameraEntity = newCam.GetID();
		out->GetComponent<Tag>(newCam.GetID()).EntityName = "Camera";
		return true;



	}

	bool Scene::IsDescendant(EntityID child, EntityID parent)
	{

		if (parent == Chroma::ENTITY_NULL || child == Chroma::ENTITY_NULL)
			return false;

		auto &p_rel = GetComponent<Chroma::Transform>(parent);
		auto &c_rel = GetComponent<Chroma::Transform>(child);

		auto& cp = child;

		while (cp != Chroma::ENTITY_NULL)
		{
			auto rpp = GetComponent<Chroma::Transform>(cp).Parent;
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
		auto &r = GetComponent<Chroma::Transform>(entity);
		return r.Parent == ENTITY_NULL;
	}

	EntityID Scene::GetRootEntity(EntityID child)
	{
		if (child == ENTITY_NULL)
			return ENTITY_NULL;

		auto& c_rel = GetComponent<Chroma::Transform>(child);

		auto& cp = child;

		while (cp != Chroma::ENTITY_NULL)
		{
			auto rpp = GetComponent<Chroma::Transform>(cp).Parent;
			if (rpp == ENTITY_NULL)
				return cp;
			cp = rpp;
		}
		return ENTITY_NULL;
	}


	std::vector<Chroma::Reflection::Type> Scene::GetComponentTypes()
	{
		std::vector<Chroma::Reflection::Type> types;

		auto comp_type = Chroma::Reflection::Resolve<Component>();

		for (auto type : Chroma::Reflection::Resolve())
		{
			if (type.Is<Component>())
				continue;
			if (type.IsAbstract())
				continue;
			CHROMA_CORE_TRACE("{}", type.GetName());
			for (auto& base : type.BasesRecursive())
			{
				CHROMA_CORE_TRACE("\t{}", base.GetName());
			}
			//if (type.Base(comp_type.Id()).Valid())
			//	types.push_back(type);
		}

		return types;
	}

	std::vector<Component*> Scene::GetAllComponents(EntityID entity)
	{
		std::vector<Component*> retval;
		for (auto& [type, func] : ComponentRegistry::ComponentGetFunctions())
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

		out << YAML::Key << "Components" << YAML::Value << YAML::BeginSeq;

		std::vector<Component*> components = this->GetAllComponents(entity);
		std::sort(components.begin(), components.end(), [](Component* a, Component* b) { return a->order_id < b->order_id; });

		for (Component* comp : components)
		{
			if (comp->IsType<Tag>())
			{
				continue;
			}
			else
			{
				Reflection::SerializeObjectYAML(out, comp->ToAny());
			}

		}

		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	void Scene::OnLoad()
	{
		physics_system->PreLoad();
		scripting_system->PreLoad();
		audio_system->PreLoad();
		sprite_renderer_system->PreLoad();
		particle_system->PreLoad();
		camera_system->PreLoad();

		physics_system->Load();
		scripting_system->Load();
		audio_system->Load();
		sprite_renderer_system->Load();
		particle_system->Load();
		camera_system->Load();

		physics_system->PostLoad();
		scripting_system->PostLoad();
		audio_system->PostLoad();
		sprite_renderer_system->PostLoad();
		particle_system->PostLoad();
		camera_system->PostLoad();
	}

	void Scene::Init()
	{
		scripting_system->EarlyInit();
		physics_system->EarlyInit();
		audio_system->EarlyInit();
		sprite_renderer_system->EarlyInit();
		particle_system->EarlyInit();
		camera_system->EarlyInit();

		scripting_system->Init();
		physics_system->Init();
		audio_system->Init();
		sprite_renderer_system->Init();
		particle_system->Init();
		camera_system->Init();

		scripting_system->LateInit();
		physics_system->LateInit();
		audio_system->LateInit();
		sprite_renderer_system->LateInit();
		particle_system->LateInit();
		camera_system->LateInit();
	}

	void Scene::Update(Time delta)
	{
		scripting_system->EarlyUpdate(delta);
		physics_system->EarlyUpdate(delta);
		audio_system->EarlyUpdate(delta);
		sprite_renderer_system->EarlyUpdate(delta);
		particle_system->EarlyUpdate(delta);
		camera_system->EarlyUpdate(delta);

		scripting_system->Update(delta);
		physics_system->Update(delta);
		audio_system->Update(delta);
		sprite_renderer_system->Update(delta);
		particle_system->Update(delta);
		camera_system->Update(delta);

		scripting_system->LateUpdate(delta);
		physics_system->LateUpdate(delta);
		audio_system->LateUpdate(delta);
		sprite_renderer_system->LateUpdate(delta);
		particle_system->LateUpdate(delta);
		camera_system->LateUpdate(delta);
	}

	void Scene::Draw(Time delta)
	{
		scripting_system->EarlyDraw(delta);
		audio_system->EarlyDraw(delta);
		sprite_renderer_system->EarlyDraw(delta);
		particle_system->EarlyDraw(delta);
		camera_system->EarlyDraw(delta);
		physics_system->EarlyDraw(delta);

		scripting_system->Draw(delta);
		audio_system->Draw(delta);
		sprite_renderer_system->Draw(delta);
		particle_system->Draw(delta);
		camera_system->Draw(delta);
		physics_system->Draw(delta);

		scripting_system->LateDraw(delta);
		audio_system->LateDraw(delta);
		sprite_renderer_system->LateDraw(delta);
		particle_system->LateDraw(delta);
		camera_system->LateDraw(delta);
		physics_system->LateDraw(delta);
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

	std::vector<Collider*> Scene::GetColliders(EntityID entity)
	{
		std::vector<Collider*> colliders;

		CircleCollider* circleCollider = Registry.try_get<CircleCollider>(entity);
		if (circleCollider != nullptr)
			colliders.push_back(dynamic_cast<Chroma::Collider*>(circleCollider));
		EdgeCollider *edgeCollider = Registry.try_get<EdgeCollider>(entity);
		if (edgeCollider != nullptr)
			colliders.push_back(dynamic_cast<Chroma::Collider*>(edgeCollider));
		PolygonCollider *polygonCollider = Registry.try_get<PolygonCollider>(entity);
		if (polygonCollider != nullptr)
			colliders.push_back(dynamic_cast<Chroma::Collider*>(polygonCollider));
		RectangleCollider *rectangleCollider = Registry.try_get<RectangleCollider>(entity);
		if (rectangleCollider != nullptr)
			colliders.push_back(dynamic_cast<Chroma::Collider*>(rectangleCollider));

		return colliders;



	}

}