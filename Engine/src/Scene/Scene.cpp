#include "Scene.hpp"

#include "Components.hpp"

namespace Engine
{
    Scene::Scene()
    {
        registry.on_construct<entt::entity>().connect<&entt::registry::emplace<Component::Relationship>>();
        registry.on_construct<entt::entity>().connect<&entt::registry::emplace<Component::Name>>();
    }

    Entity Scene::CreateEntity()
    {
        return { registry.create(), &registry };
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