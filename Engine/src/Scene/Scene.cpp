#include "Scene.hpp"

#include "Components.hpp"

namespace Engine
{

    void CopyEntities(entt::registry& destination, const entt::registry& source)
    {
        const auto entities = source.storage<entt::entity>();
        destination.storage<entt::entity>().push(entities->cbegin(), entities->cend());
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

    template<typename T>
    void CopyComponent(entt::registry& destination, const entt::registry& source)
    {
        const auto components = source.storage<T>();

        if (! components)
        {
            return;
        }

        destination.insert<T>(components->entt::sparse_set::begin(), components->entt::sparse_set::end(), components->begin());
    }

    Scene& Scene::operator=(const Scene& other)
    {
        Resource::operator=(other);

        registry.clear();

        CopyEntities(registry, other.registry);
        CopyComponents(Component::Serializable{}, registry, other.registry);

        return *this;
    }

    std::unordered_map<entt::entity, entt::entity> MapEntities(entt::registry& destination, const entt::registry& source)
    {
        std::unordered_map<entt::entity, entt::entity> map{};

        const auto entities = source.storage<entt::entity>();

        for (auto [entity]: entities->each())
        {
            map[entity] = destination.create();
        }

        return map;
    }

    entt::entity MapEntity(const entt::entity& source, const std::unordered_map<entt::entity, entt::entity>& map)
    {
        if (const auto it = map.find(source); it != map.end())
        {
            return it->second;
        }

        return entt::null;
    }

    template<typename T>
    void MapComponent(entt::registry& destination, const entt::registry& source, const std::unordered_map<entt::entity, entt::entity>& map)
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

            if constexpr (std::is_same_v<T, Component::Relationship>)
            {
                copy.first = MapEntity(component.first, map);
                copy.prev = MapEntity(component.prev, map);
                copy.next = MapEntity(component.next, map);
                copy.parent = MapEntity(component.parent, map);
            }

            destination.emplace<T>(mapped, copy);
        }
    }

    template<typename... T>
    void MapComponents(Component::Group<T...>, entt::registry& destination, const entt::registry& source, const std::unordered_map<entt::entity, entt::entity>& map)
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
        entity.AddComponent<Component::Relationship>();

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

    void Scene::Update()
    {
        ForEachEntity<Component::Delete>([&](Entity entity) {
            registry.destroy(entity);
        });
    }
}
