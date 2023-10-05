#include "Scene.hpp"

#include "Components.hpp"

namespace Engine
{
    Scene::Scene()
    {
        registry.on_construct<entt::entity>().connect<&Scene::OnCreateEntity>(this);
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

    void Scene::OnCreateEntity(entt::registry& registry, entt::entity handle)
    {
        Entity entity{ handle, &registry };

        entity.AddComponent<Component::Name>();
        entity.AddComponent<Component::Relationship>();
    }
}