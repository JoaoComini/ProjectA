#include "SceneGraph.h"

namespace Engine
{
    SceneGraph::SceneGraph()
    {
        OnComponentAdded<Component::Transform, &SceneGraph::AddOrReplaceComponent<Component::LocalToWorld>>(this);
    }

    std::unordered_map<entt::entity, Entity::Id> MapEntities(const entt::registry& source, entt::registry& destination)
    {
        std::unordered_map<entt::entity, Entity::Id> map{};

        for (const auto entities = source.storage<entt::entity>(); const auto& [entity]: entities->each())
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
    void MapComponent(const entt::registry& source, entt::registry& destination, const std::unordered_map<entt::entity, Entity::Id>& map)
    {
        const auto components = source.storage<T>();

        if (!components)
        {
            return;
        }

        for (auto [entity, component] : components->each())
        {
            auto mapped = MapEntity(entity, map);
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
    void MapComponentGroup(const entt::registry& source, entt::registry& destination, const std::unordered_map<entt::entity, Entity::Id>& map, Component::Group<T...>)
    {
        ([&]()
            {
                MapComponent<T>(source, destination, map);
            }(),
        ...);
    }

    void SceneGraph::Add(const SceneResource& scene)
    {
        const auto map = MapEntities(scene.registry, registry);
        MapComponentGroup(scene.registry, registry, map, Component::Serializable);
    }

    std::unique_ptr<SceneResource> SceneGraph::Pack() const
    {
        auto resource = std::make_unique<SceneResource>();

        auto map = MapEntities(registry, resource->registry);
        MapComponentGroup(registry, resource->registry, map, Component::Serializable);

        return resource;
    }

    void SceneGraph::Update()
    {
        {
            auto query = Query<Component::Delete>();

            for (const auto entity : query)
            {
                registry.destroy(entity);
            }
        }

        {
            auto query = Query<Component::Transform>(Exclusion<Component::Hierarchy>);

            for (auto entity : query)
            {
                ComputeEntityLocalToWorld({}, entity);
            }
        }
    }

    void SceneGraph::ComputeEntityLocalToWorld(const Component::LocalToWorld& parent, Entity::Id entity)
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

    void SceneGraph::Pause()
    {
        paused = true;
    }

    void SceneGraph::Resume()
    {
        paused = false;
    }

    bool SceneGraph::IsPaused() const
    {
        return paused;
    }
}
