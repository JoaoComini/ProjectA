#include "Entity.hpp"

#include "Components.hpp"

namespace Engine
{
    Entity::Entity(entt::entity handle, entt::registry* registry) : handle(handle), registry(registry)
    {

    }

    void Entity::SetParent(Entity newParent)
    {
        if (!newParent && !HasComponent<Component::Hierarchy>())
        {
            return;
        }

        auto& comp = GetOrAddComponent<Component::Hierarchy>();

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
            return;
        }

        RemoveComponent<Component::Hierarchy>();
    }

    void Entity::AddChild(Entity entity)
    {
        auto& children = GetOrAddComponent<Component::Children>();
        auto& hierarchy = entity.GetComponent<Component::Hierarchy>();

        hierarchy.parent = *this;
        hierarchy.next = children.first;

        if (children.first)
        {
            registry->get<Component::Hierarchy>(children.first).prev = entity;
        }

        children.first = entity;
        children.size += 1;
    }

    void Entity::RemoveChild(Entity entity)
    {
        auto& children = GetComponent<Component::Children>();
        auto& hierarchy = entity.GetComponent<Component::Hierarchy>();

        hierarchy.parent = {};

        if (children.first == entity)
        {
            children.first = hierarchy.next;
        }

        if (hierarchy.next)
        {
            registry->get<Component::Hierarchy>(hierarchy.next).prev = hierarchy.prev;
        }

        if (hierarchy.prev)
        {
            registry->get<Component::Hierarchy>(hierarchy.next).next = hierarchy.next;
        }

        children.size -= 1;
        
        if (children.size <= 0)
        {
            RemoveComponent<Component::Children>();
        }
    }

    void Entity::SetName(const std::string& name)
    {
        GetComponent<Component::Name>().name = name;
    }

    std::string_view Entity::GetName() const
    {
        return GetComponent<Component::Name>().name;
    }

    Entity Entity::GetParent() const
    {
        if (auto hierarchy = TryGetComponent<Component::Hierarchy>())
        {
            return { hierarchy->parent, registry };
        }

        return {};
    }

    std::vector<Entity> Entity::GetChildren() const
    {
        std::vector<Entity> children;

        if(! HasComponent<Component::Children>())
        {
            return children;
        }

        const auto& comp = GetComponent<Component::Children>();

        auto current = comp.first;
        for (size_t i = 0; i < comp.size; i++)
        {
            children.push_back(current);

            current = registry->get<Component::Hierarchy>(current).next;
        }

        return children;
    }
}