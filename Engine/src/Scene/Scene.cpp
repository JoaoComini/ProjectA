#include "Scene.h"

namespace Engine
{
    Entity::Id Scene::CreateEntity()
    {
        auto entity = registry.create();

        AddComponent<Component::Name>(entity);
        AddComponent<Component::Transform>(entity);

        return entity;
    }

    void Scene::DestroyEntity(Entity::Id entity)
    {
        if (auto hierarchy = TryGetComponent<Component::Hierarchy>(entity))
        {
            RemoveChild(hierarchy->parent, entity);
        }

        if (auto children = TryGetComponent<Component::Children>(entity))
        {
            auto curr = children->first;

            while (Valid(curr))
            {
                auto& hierarchy = GetComponent<Component::Hierarchy>(curr);

                DestroyEntity(curr);

                curr = hierarchy.next;
            }
        }

        AddComponent<Component::Delete>(entity);
    }

    bool Scene::Valid(Entity::Id entity) const
    {
        return registry.valid(entity);
    }

    void Scene::SetParent(Entity::Id entity, Entity::Id parent)
    {
        if (! Valid(parent))
        {
            if (auto hierarchy = TryGetComponent<Component::Hierarchy>(entity))
            {
                RemoveChild(hierarchy->parent, entity);
                RemoveComponent<Component::Hierarchy>(entity);
            }

            return;
        }

        auto& comp = GetOrAddComponent<Component::Hierarchy>(entity);

        if (comp.parent == parent)
        {
            return;
        }

        if (Valid(comp.parent))
        {
            RemoveChild(comp.parent, entity);
        }

        AddChild(parent, entity);
    }

    void Scene::AddChild(Entity::Id parent, Entity::Id child)
    {
        auto& children = GetOrAddComponent<Component::Children>(parent);
        auto& hierarchy = GetComponent<Component::Hierarchy>(child);

        hierarchy.parent = parent;
        hierarchy.next = children.first;

        if (Valid(children.first))
        {
            GetComponent<Component::Hierarchy>(children.first).prev = child;
        }

        children.first = child;
        children.size += 1;
    }

    void Scene::RemoveChild(Entity::Id parent, Entity::Id child)
    {
        auto& children = GetComponent<Component::Children>(parent);
        auto& hierarchy = GetComponent<Component::Hierarchy>(child);

        hierarchy.parent = Entity::Null;

        if (children.first == child)
        {
            children.first = hierarchy.next;
        }

        if (Valid(hierarchy.next))
        {
            GetComponent<Component::Hierarchy>(hierarchy.next).prev = hierarchy.prev;
        }

        if (Valid(hierarchy.prev))
        {
            GetComponent<Component::Hierarchy>(hierarchy.prev).next = hierarchy.next;
        }

        children.size -= 1;

        if (children.size <= 0)
        {
            RemoveComponent<Component::Children>(parent);
        }
    }


    Entity::Id Scene::GetParent(Entity::Id entity)
    {
        if (auto hierarchy = TryGetComponent<Component::Hierarchy>(entity))
        {
            return hierarchy->parent;
        }

        return Entity::Null;
    }
}