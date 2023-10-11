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

        childComp.parent = *this;
        childComp.next = thisComp.first;

        if (thisComp.first)
        {
            thisComp.first.GetComponent<Component::Relationship>().prev = entity;
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

        if (childComp.next)
        {
            childComp.next.GetComponent<Component::Relationship>().prev = childComp.prev;
        }

        if (childComp.prev)
        {
            childComp.prev.GetComponent<Component::Relationship>().next = childComp.next;
        }

        thisComp.children -= 1;
    }

    Entity Entity::GetParent() const
    {
        return GetComponent<Component::Relationship>().parent;
    }

    std::vector<Entity> Entity::GetChildren() const
    {
        std::vector<Entity> children;

        const auto& comp = GetComponent<Component::Relationship>();

        auto current = comp.first;
        for (size_t i = 0; i < comp.children; i++)
        {
            children.push_back(current);

            current = current.GetComponent<Component::Relationship>().next;
        }

        return children;
    }

    Uuid Entity::GetId() const
    {
        return GetComponent<Component::Id>().id;
    }

}