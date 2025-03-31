#include "Scene.h"

#include "Component/Group.h"

namespace Engine
{
    Entity::Id Scene::CreateEntity()
    {
        const auto entity = registry.create();

        AddComponent<Component::Name>(entity);
        AddComponent<Component::Transform>(entity);

        return entity;
    }

    void Scene::DestroyEntity(Entity::Id entity)
    {
        if (const auto hierarchy = TryGetComponent<Component::Hierarchy>(entity))
        {
            RemoveChild(hierarchy->parent, entity);
        }

        if (const auto children = TryGetComponent<Component::Children>(entity))
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

    std::unordered_map<entt::entity, Entity::Id> MapEntities(const entt::registry& from, entt::registry& to)
    {
        std::unordered_map<entt::entity, Entity::Id> map{};

        for (const auto entities = from.storage<entt::entity>(); const auto& [entity]: entities->each())
        {
            map[entity] = to.create();
        }

        return map;
    }

    Entity::Id MapEntity(const entt::entity& from, const std::unordered_map<entt::entity, Entity::Id>& map)
    {
        if (const auto it = map.find(from); it != map.end())
        {
            return it->second;
        }

        return Entity::Null;
    }

    template<typename T>
    void MapComponent(const entt::registry& from, entt::registry& to, const std::unordered_map<entt::entity, Entity::Id>& map)
    {
        const auto components = from.storage<T>();

        if (!components)
        {
            return;
        }

        for (auto [entity, component] : components->reach())
        {
            auto mapped = MapEntity(entity, map);

            if (mapped == Entity::Null)
            {
                continue;
            }

            auto copy = component;

            if constexpr (std::is_same_v<T, Component::Hierarchy>)
            {
                copy.prev = MapEntity(component.prev, map);
                copy.next = MapEntity(component.next, map);
                copy.parent = MapEntity(component.parent, map);
            }

            if constexpr (std::is_same_v<T, Component::Children>)
            {
                copy.first = MapEntity(component.first, map);
            }

            to.emplace<T>(mapped, copy);
        }
    }

    template<typename... T>
    void MapComponentGroup(const entt::registry& from, entt::registry& to, const std::unordered_map<entt::entity, Entity::Id>& map, Component::Group<T...>)
    {
        ([&]()
            {
                MapComponent<T>(from, to, map);
            }(),
        ...);
    }

    std::unordered_map<Entity::Id, Entity::Id> Scene::Map(const Scene& from, Scene& to)
    {
        const auto map = MapEntities(from.registry, to.registry);
        MapComponentGroup(from.registry, to.registry, map, Component::Serializable);

        return map;
    }

    void CopyEntities(const entt::registry& from, entt::registry& to)
    {
        const auto entities = from.storage<entt::entity>();

        to.storage<entt::entity>().push(entities->rbegin(), entities->rend());
        to.storage<entt::entity>().free_list(entities->free_list());
    }

    template<typename T>
    void CopyComponent(const entt::registry& from, entt::registry& to)
    {
        const auto components = from.storage<T>();

        if (! components)
        {
            return;
        }

        to.storage<T>().insert(components->entt::sparse_set::rbegin(), components->entt::sparse_set::rend(), components->rbegin());
    }


    template<typename... T>
    void CopyComponentGroup(const entt::registry& from, entt::registry& to, Component::Group<T...>)
    {
        ([&]()
            {
                CopyComponent<T>(from, to);
            }(),
        ...);
    }


    void Scene::Copy(const Scene& from, Scene& to)
    {
        for([[maybe_unused]] auto [_, storage] : to.registry.storage()) {
            assert(storage.empty() && "Scene must be empty");
        }

        CopyEntities(from.registry, to.registry);
        CopyComponentGroup(from.registry, to.registry, Component::Serializable);
    }
}