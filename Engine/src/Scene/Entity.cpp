#include "Entity.hpp"

#include "Components.hpp"

namespace Engine
{
    Entity::Entity(entt::entity handle, entt::registry* registry) : handle(handle), registry(registry)
    {

    }

    void Entity::SetParent(Entity newParent)
    {
        auto& comp = GetComponent<Component::Relationship>();

        if (comp.parent == newParent)
        {
            return;
        }

        Entity oldParent = GetParent();

        if (oldParent)
        {
            oldParent.RemoveChild(*this);
        }

        if (newParent)
        {
            newParent.AddChild(*this);
        }
    }

    void Entity::AddChild(Entity entity)
    {
        auto& thisComp = GetComponent<Component::Relationship>();
        auto& childComp = entity.GetComponent<Component::Relationship>();

        childComp.parent = handle;
        childComp.next = thisComp.first;

        if (thisComp.first != entt::null)
        {
            registry->get<Component::Relationship>(thisComp.first).prev = entity;
        }

        thisComp.first = entity;
        thisComp.children += 1;
    }

    void Entity::RemoveChild(Entity entity)
    {
        auto& thisComp = GetComponent<Component::Relationship>();
        auto& childComp = entity.GetComponent<Component::Relationship>();

        childComp.parent = {};

        if (thisComp.first == entity)
        {
            thisComp.first = childComp.next;
        }

        if (childComp.next != entt::null)
        {
            registry->get<Component::Relationship>(childComp.next).prev = childComp.prev;
        }

        if (childComp.prev != entt::null)
        {
            registry->get<Component::Relationship>(childComp.next).next = childComp.next;
        }

        thisComp.children -= 1;
    }

    Entity Entity::GetParent() const
    {
        return { GetComponent<Component::Relationship>().parent, registry };
    }

    std::vector<Entity> Entity::GetChildren() const
    {
        std::vector<Entity> children;

        const auto& comp = GetComponent<Component::Relationship>();

        auto current = comp.first;
        for (size_t i = 0; i < comp.children; i++)
        {
            children.push_back({ current, registry });

            current = registry->get<Component::Relationship>(current).next;
        }

        return children;
    }
}