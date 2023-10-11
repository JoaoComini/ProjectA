#include "Scene.hpp"

#include "Components.hpp"

namespace Engine
{
    Entity Scene::CreateEntity(Uuid id)
    {
        Entity entity{ registry.create(), &registry };

        entity.AddComponent<Component::Name>();
        entity.AddComponent<Component::Relationship>();

        entity.AddComponent<Component::Id>().id = id;
        entityMap[id] = entity;

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

    Entity Scene::FindEntityById(Uuid id)
    {
        if (entityMap.find(id) != entityMap.end())
        {
            return entityMap[id];
        }

        return {};
    }

    void Scene::Update()
    {
        ForEachEntity<Component::Delete>([&](Entity entity) {
            registry.destroy(entity);
            entityMap.erase(entity.GetId());
        });
    }
}
