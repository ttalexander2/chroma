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
#include <mirr/mirr.h>

#include <ranges>


namespace Chroma
{
	Scene::Scene() :
		ID(GUID::CreateGUID())
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

	Scene *Scene::Copy()
	{
		auto out = new Scene();
		out->EntityOrder = std::vector<EntityID>(EntityOrder);
		out->ID = ID;
		out->Layers = std::vector<Layer>(Layers);
		for (auto oldEntity : Registry.view<Transform>())
		{
			auto newEntity = out->Registry.create(oldEntity);
			for (Component *c : GetAllComponents(oldEntity))
			{
				if (c != nullptr && c->GetType().valid())
				{
					auto func = ComponentRegistry::ComponentCopyFunctions()[c->GetType().id()];
					if (func)
					{
						func(newEntity, &out->Registry, &Registry);
					}
					
				}
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

		auto &child_r = e.GetComponent<Transform>();
		auto &parent_r = GetComponent<Transform>(static_cast<EntityID>(parent));
		parent_r.Children.push_back(e.GetID());
		child_r.Parent = static_cast<EntityID>(parent);
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
		if (!Registry.valid(entity))
			return Math::vec2();

		const Transform* transform = Registry.try_get<Transform>(entity);

		if (transform == nullptr)
		{
			CHROMA_CORE_WARN("Entity {} does not contain a transform! Something went very wrong.", entity);
			return Math::vec2();
		}
		
		if (!transform->IsChild())
			return transform->Position;
		Math::vec2 finalPos = transform->Position;
		auto parentID = transform->Parent;
		while (parentID != ENTITY_NULL)
		{
			const Transform* parentTransform = Registry.try_get<Transform>(parentID);
			finalPos += parentTransform->Position;
			parentID = parentTransform->Parent;
		}
		return finalPos;
	}

	void Scene::SetTransformAbsolutePosition(EntityID entity, const Math::vec2 &position)
	{
		if (!Registry.valid(entity))
			return;

		Transform &transform = Registry.get<Transform>(entity);
		if (!transform.IsChild())
		{
			transform.Position = Math::vec2(position.x, position.y);
			return;
		}
		Math::vec2 finalPos = position;
		auto parentID = transform.Parent;
		while (parentID != ENTITY_NULL)
		{
			Transform &parentTransform = Registry.get<Transform>(parentID);
			finalPos -= parentTransform.Position;
			parentID = parentTransform.Parent;
		}
		transform.Position = finalPos;
	}

	void Scene::DestroyEntity(EntityID entity, bool destroy_children)
	{
		auto &rel = Registry.get<Transform>(entity);
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
				auto &crel = Registry.get<Transform>(child);
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

	EntityID Scene::FindChildByName(EntityID entity, const std::string &child_name)
	{
		if (!Registry.valid(entity))
		{
			CHROMA_CORE_ERROR("Entity [{}] not valid!", entity);
			return ENTITY_NULL;
		}

		auto &rel = Registry.get<Transform>(entity);
		for (auto child : rel.Children)
		{
			auto &tag = Registry.get<Tag>(entity);
			if (tag.EntityName == child_name)
				return child;
		}

		return ENTITY_NULL;
	}

	EntityID Scene::GetFirstChild(EntityID entity)
	{
		if (!Registry.valid(entity))
		{
			CHROMA_CORE_ERROR("Entity [{}] not valid!", entity);
			return ENTITY_NULL;
		}

		auto &rel = Registry.get<Transform>(entity);
		if (!rel.HasChildren() || rel.Children.size() <= 0)
		{
			CHROMA_CORE_ERROR("Entity [{}] does not have children!", entity);
			return ENTITY_NULL;
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

	Entity Scene::FindEntityByName(const std::string &name)
	{
		for (auto entity : Registry.view<Tag>())
		{
			Tag &tag = Registry.get<Tag>(entity);
			if (name == tag.EntityName)
			{
				return Entity(entity, this);
			}
		}
		return Entity(ENTITY_NULL);
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

		for (auto &e : EntityOrder)
		{
			out << static_cast<uint32_t>(e);
		}

		out << YAML::EndSeq;

		out << YAML::Key << "Layers" << YAML::Value << YAML::BeginSeq;
		for (auto &layer : Layers)
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

	bool Scene::Deserialize(Scene *out, const std::string &yaml, bool load_assets)
	{
		uint64_t maxId = 0;
		auto data = YAML::Load(yaml);
		if (!data["Scene"])
			return false;

		auto sceneName = data["Scene"].as<std::string>();
		CHROMA_CORE_TRACE("Deserializing Scene '{}'", sceneName);

		if (!data["GUID"])
			return false;

		auto guid_string = data["GUID"].as<std::string>();
		GUID guid = GUID::Parse(guid_string);

		if (guid == GUID::Zero())
			guid = GUID::CreateGUID();

		auto cam = data["Primary Camera"];
		if (cam)
		{
			out->PrimaryCameraEntity = static_cast<EntityID>(cam.as<uint32_t>());
		}
		else
		{
			auto newCam = out->NewEntity();
			out->AddComponent<Camera>(newCam.GetID());
			out->PrimaryCameraEntity = newCam.GetID();
			out->GetComponent<Tag>(newCam.GetID()).EntityName = "Camera";
		}

		out->ID = guid;
		out->Name = sceneName;

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				auto id = static_cast<EntityID>(entity["Entity"].as<uint32_t>());

				EntityID newEntity = out->Registry.create(id);
				auto name = entity["Name"];
				Tag &tag = out->Registry.get_or_emplace<Tag>(newEntity);
				if (name)
					tag.EntityName = name.as<std::string>();

				CHROMA_CORE_TRACE("Deserialized Entity with ID = {0} (hint={1}) (name={2})", newEntity, id, tag.EntityName);
				DeserializeEntity(id, entity, out);
			}
		}
		//
		auto order = data["RootEntities"];
		if (order)
		{
			for (auto id : order)
			{
				out->EntityOrder.push_back(static_cast<EntityID>(id.as<uint32_t>()));
			}
		}

		out->Layers.clear();
		auto layers = data["Layers"];
		if (layers)
		{
			for (auto layer : layers)
			{
				auto l = Layer("", 0);
				l.Deserialize(layer);
				out->Layers.push_back(l);
			}
		}

		for (auto e : out->Registry.view<Transform>())
		{
			Transform &transform = out->Registry.get<Transform>(e);
			EntityID parent = transform.Parent;
			if (out->Registry.valid(parent))
			{
				Transform *parent_transform = out->Registry.try_get<Transform>(parent);
				if (parent_transform)
				{
					parent_transform->Children.push_back(e);
				}
			}
		}

		return true;
	}

	bool Scene::IsDescendant(EntityID child, EntityID parent)
	{
		if (parent == ENTITY_NULL || child == ENTITY_NULL)
			return false;

		auto &p_rel = GetComponent<Transform>(parent);
		auto &c_rel = GetComponent<Transform>(child);

		auto &cp = child;

		while (cp != ENTITY_NULL)
		{
			auto rpp = GetComponent<Transform>(cp).Parent;
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
		auto &r = GetComponent<Transform>(entity);
		return r.Parent == ENTITY_NULL;
	}

	EntityID Scene::GetRootEntity(EntityID child)
	{
		if (child == ENTITY_NULL)
			return ENTITY_NULL;

		auto &c_rel = GetComponent<Transform>(child);

		auto &cp = child;

		while (cp != ENTITY_NULL)
		{
			auto rpp = GetComponent<Transform>(cp).Parent;
			if (rpp == ENTITY_NULL)
				return cp;
			cp = rpp;
		}
		return ENTITY_NULL;
	}


	std::vector<Reflection::type> Scene::GetComponentTypes()
	{
		std::vector<Reflection::type> types;

		auto comp_type = Reflection::resolve<Component>();

		for (auto type : Reflection::resolve())
		{
			if (type.is<Component>())
				continue;
			if (type.is_abstract())
				continue;
			//CHROMA_CORE_TRACE("{}", type.GetName());
			for (const auto id : type.bases())
			{
				//CHROMA_CORE_TRACE("\t{}", base.GetName());
				if (Reflection::type base = Reflection::resolve(id); base.is<Component>())
				{
					types.push_back(type);
					break;
				}
			}
			//if (type.Base(comp_type.Id()).Valid())
			//	types.push_back(type);
		}

		return types;
	}

	std::vector<Component *> Scene::GetAllComponents(EntityID entity)
	{
		std::vector<Component *> retval;
		for (auto &func : ComponentRegistry::ComponentGetFunctions() | std::views::values)
		{
			Component *comp = func(entity, &Registry);
			if (comp != nullptr)
				retval.push_back(comp);
		}

		return retval;
	}

	void Scene::SerializeEntity(YAML::Emitter &out, EntityID entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << static_cast<uint32_t>(entity);

		Tag &tag = Registry.get_or_emplace<Tag>(entity);
		out << YAML::Key << "Name" << YAML::Value << tag.EntityName;

		out << YAML::Key << "Components" << YAML::Value << YAML::BeginSeq;

		std::vector<Component *> components = this->GetAllComponents(entity);
		std::sort(components.begin(), components.end(), [](Component *a, Component *b) { return a->order_id < b->order_id; });

		for (Component *comp : components)
		{
			if (comp->TypeInfo.is<Tag>())
			{
				continue;
			}
			out << YAML::BeginMap << YAML::Key;
			out << comp->GetType().name();
			out << YAML::Value;
			Reflection::Serializer::SerializeObjectYAML(out, comp->ToHandle());
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	void Scene::DeserializeEntity(EntityID id, YAML::Node &node, Scene *out)
	{
		auto components = node["Components"];
		if (components)
		{
			for (auto component_map : components)
			{
				for (auto component : component_map)
				{
					auto key = component.first.as<std::string>();
					//CHROMA_CORE_INFO("{}", key);
					
					auto type = Reflection::resolve(key);
					if (!type.valid())
						CHROMA_CORE_ERROR("Type {} not valid!", key);
					
					Component *comp = ComponentRegistry::AddComponent(key, id, &out->Registry);

					if (!comp)
						CHROMA_CORE_ERROR("Component construction with entity id failed!");

					//Reflection::Serializer::DeserializeObjectYAML(comp->ToHandle(), component.second);
				}
			}
		}
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

	Camera &Scene::GetPrimaryCamera()
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
		Camera &camera = Registry.get_or_emplace<Camera>(PrimaryCameraEntity, PrimaryCameraEntity);
		return true;
	}

	std::vector<Collider *> Scene::GetColliders(EntityID entity)
	{
		std::vector<Collider *> colliders;

		CircleCollider *circleCollider = Registry.try_get<CircleCollider>(entity);
		if (circleCollider != nullptr)
			colliders.push_back(circleCollider);
		EdgeCollider *edgeCollider = Registry.try_get<EdgeCollider>(entity);
		if (edgeCollider != nullptr)
			colliders.push_back(edgeCollider);
		PolygonCollider *polygonCollider = Registry.try_get<PolygonCollider>(entity);
		if (polygonCollider != nullptr)
			colliders.push_back(polygonCollider);
		RectangleCollider *rectangleCollider = Registry.try_get<RectangleCollider>(entity);
		if (rectangleCollider != nullptr)
			colliders.push_back(rectangleCollider);

		return colliders;
	}
}
