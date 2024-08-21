#include "Scene.hpp"

#include "Components.hpp"

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
            auto copy = component;

            if constexpr (std::is_same_v<T, Component::Hierarchy>)
            {
                copy.prev = { component.prev, &destination };
                copy.next = { component.next, &destination };
                copy.parent = { component.parent, &destination };
            }

            if constexpr (std::is_same_v<T, Component::Children>)
            {
                copy.first = { component.first, &destination };
            }

            destination.emplace<T>(entity, copy);
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

    std::unordered_map<entt::entity, Entity> MapEntities(entt::registry& destination, const entt::registry& source)
    {
        std::unordered_map<entt::entity, Entity> map{};

        const auto entities = source.storage<entt::entity>();

        for (auto [entity]: entities->each())
        {
            map[entity] = { destination.create(), &destination };
        }

        return map;
    }

    Entity MapEntity(const entt::entity& source, const std::unordered_map<entt::entity, Entity>& map)
    {
        if (const auto it = map.find(source); it != map.end())
        {
            return it->second;
        }

        return {};
    }

    template<typename T>
    void MapComponent(entt::registry& destination, const entt::registry& source, const std::unordered_map<entt::entity, Entity>& map)
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
    void MapComponents(Component::Group<T...>, entt::registry& destination, const entt::registry& source, const std::unordered_map<entt::entity, Entity>& map)
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

    Entity Scene::CreateEntity()
    {
        Entity entity{ registry.create(), &registry };

        entity.AddComponent<Component::Name>();
        entity.AddComponent<Component::Transform>();

        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        entity.SetParent({});
        entity.AddComponent<Component::Delete>();

        for (auto& child : entity.GetChildren())
        {
            DestroyEntity(child);
        }
    }

    Entity Scene::FindEntityByHandle(uint32_t handle)
    {
        auto entity = static_cast<entt::entity>(handle);

        if (! registry.valid(entity))
        {
            return {};
        }

        return { entity, &registry };
    }

    void Scene::Update()
    {
        ForEachEntity<Component::Delete>([&](auto entity) {
            registry.destroy(entity);
        });

        ForEachEntity<Component::Transform>([&](Entity entity) {
            ComputeEntityLocalToWorld({}, entity);
        }, Exclusion<Component::Hierarchy>{});

    }

    void Scene::ComputeEntityLocalToWorld(const Component::LocalToWorld& parent, Entity entity)
    {
        auto& transform = entity.GetComponent<Component::Transform>();
        auto& localToWorld = entity.GetComponent<Component::LocalToWorld>();

        localToWorld.value = parent.value * transform.GetLocalMatrix();

        if (!entity.HasComponent<Component::Children>())
        {
            return;
        }

        const auto& children = entity.GetComponent<Component::Children>();
        auto current = children.first;

        auto& storage = registry.storage<Component::Hierarchy>();

        for (size_t i = 0; i < children.size; i++)
        {
            ComputeEntityLocalToWorld(localToWorld, { current, &registry });

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
