#include "chromapch.h"
#include "ECS.h"


namespace Chroma
{
    std::unordered_map<size_t, std::function<Component* (EntityID, entt::registry*)> > ECS::ComponentFactory_Get;
    std::unordered_map<size_t, std::function<Component* (EntityID, entt::registry*)> > ECS::ComponentFactory_Add;
    std::unordered_map<size_t, std::function<size_t(EntityID, entt::registry*)>> ECS::ComponentFactory_Remove;
    std::unordered_map<size_t, std::function<bool(EntityID, entt::registry*)>> ECS::ComponentFactory_Has;


    std::unordered_set<std::string> ECS::Components;

    std::hash<std::string> ECS::Hash;

    std::unordered_set<std::string> ECS::GetComponentNames()
    {
        return Components;
    }
    Component* ECS::GetComponent(std::string name, EntityID entity, entt::registry* registry)
    {
        return ComponentFactory_Get[Hash(name)](entity, registry);
    }

    Component* ECS::AddComponent(std::string name, EntityID entity, entt::registry* registry)
    {
        return ComponentFactory_Add[Hash(name)](entity, registry);
    }

    size_t ECS::RemoveComponent(std::string name, EntityID entity, entt::registry* registry)
    {
        return ComponentFactory_Remove[Hash(name)](entity, registry);
    }

    bool ECS::HasComponent(std::string name, EntityID entity, entt::registry* registry)
    {
        return ComponentFactory_Has[Hash(name)](entity, registry);
    }
}


