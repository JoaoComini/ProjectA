#include "Scene.h"

#include "Components.h"

namespace Engine
{

    Scene::Scene()
    {
        registry.on_construct<Component::Transform>().connect<&entt::registry::emplace_or_replace<Component::LocalToWorld>>(&registry);
    }

    void CopyEntities(entt::registry& destination, const entt::registry& source)
    {
        const auto entities = source.storage<entt::entity>();
        destination.storage<entt::entity>().push(entities->cbegin(), entities->cend());
        destination.storage<entt::entity>().in_use(entities->in_use());
    }

    template<typename T>
    void CopyComponent(entt::registry& destination, const entt::registry& source)
    {
        const auto components = source.storage<T>();

        if (! components)
        {
            return;
        }

        for (auto [entity, component] : components->each())
        {
            destination.emplace<T>(entity, component);
        }
    }

    template<typename... T>
    void CopyComponents(Component::Group<T...>, entt::registry& destination, const entt::registry& source)
    {
        ([&]()
            {
                CopyComponent<T>(destination, source);
            }(),
        ...);
    }

    Scene& Scene::operator=(const Scene& other)
    {
        Resource::operator=(other);

        registry.clear();

        CopyEntities(registry, other.registry);
        CopyComponents(Component::Serializable{}, registry, other.registry);

        return *this;
    }

    std::unordered_map<entt::entity, Entity::Id> MapEntities(entt::registry& destination, const entt::registry& source)
    {
        std::unordered_map<entt::entity, Entity::Id> map{};

        const auto entities = source.storage<entt::entity>();

        for (auto [entity]: entities->each())
        {
            map[entity] = destination.create();
        }

        return map;
    }

    Entity::Id MapEntity(const entt::entity& source, const std::unordered_map<entt::entity, Entity::Id>& map)
    {
        if (const auto it = map.find(source); it != map.end())
        {
            return it->second;
        }

        return Entity::Null;
    }

    template<typename T>
    void MapComponent(entt::registry& destination, const entt::registry& source, const std::unordered_map<entt::entity, Entity::Id>& map)
    {
        const auto components = source.storage<T>();

        if (!components)
        {
            return;
        }

        for (auto [entity, component] : components->each())
        {
            auto mapped = map.at(entity);
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

            destination.emplace<T>(mapped, copy);
        }
    }

    template<typename... T>
    void MapComponents(Component::Group<T...>, entt::registry& destination, const entt::registry& source, const std::unordered_map<entt::entity, Entity::Id>& map)
    {
        ([&]()
            {
                MapComponent<T>(destination, source, map);
            }(),
        ...);
    }

    void Scene::Add(const Scene& scene)
    {
        auto map = MapEntities(registry, scene.registry);
        MapComponents(Component::Serializable{}, registry, scene.registry, map);
    }

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

    void Scene::Update()
    {
        {
            auto query = Query<Component::Delete>();

            for (auto entity : query)
            {
                registry.destroy(entity);
            }
        }

        {
            auto query = Query<Component::Transform>(Exclusion<Component::Hierarchy>{});

            for (auto entity : query)
            {
                ComputeEntityLocalToWorld({}, entity);
            }
        }
    }

    void Scene::ComputeEntityLocalToWorld(const Component::LocalToWorld& parent, Entity::Id entity)
    {
        auto& transform = GetComponent<Component::Transform>(entity);
        auto& localToWorld = GetComponent<Component::LocalToWorld>(entity);

        localToWorld.value = parent.value * transform.GetLocalMatrix();

        if (! HasComponent<Component::Children>(entity))
        {
            return;
        }

        const auto& children = GetComponent<Component::Children>(entity);
        auto current = children.first;

        auto& storage = registry.storage<Component::Hierarchy>();

        for (size_t i = 0; i < children.size; i++)
        {
            ComputeEntityLocalToWorld(localToWorld, current);

            current = storage.get(current).next;
        }
    }

    void Scene::Pause()
    {
        paused = true;
    }

    void Scene::Resume()
    {
        paused = false;
    }

    bool Scene::IsPaused() const
    {
        return paused;
    }
}
