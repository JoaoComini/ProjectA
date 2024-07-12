#include "Scene.hpp"

#include "Components.hpp"

namespace Engine
{
    Scene& Scene::operator=(const Scene& other)
    {
        Resource::operator=(other);

        registry.clear();

        const auto entities = other.registry.storage<entt::entity>();
        registry.storage<entt::entity>().push(entities->cbegin(), entities->cend());

        CopyComponents(Component::Serializable{}, other.registry);

        return *this;
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
