#include "SceneGraph.h"

#include "Component/SceneInstance.h"

namespace Engine
{
    template<typename... T>
    void InitializeStorages(entt::registry& registry, Component::Group<T...>)
    {
        ([&]()
        {
           static_cast<void>(registry.storage<T>());
        }(), ...);
    }

    SceneGraph::SceneGraph()
    {
        OnComponentAdded<Component::Transform, &SceneGraph::AddOrReplaceComponent<Component::LocalToWorld>>(this);
        InitializeStorages(registry, Component::Serializable);
    }

    bool ShouldPackWithInstance(const entt::sparse_set& storage)
    {
        return storage.type() == entt::type_id<Component::SceneInstance>()
            || storage.type() == entt::type_id<Component::Hierarchy>()
            || storage.type() == entt::type_id<Component::Children>();
    }

    std::unique_ptr<SceneResource> SceneGraph::Pack() const
    {
        auto resource = std::make_unique<SceneResource>();

        Copy(*this, *resource);

        // Destroy all Components but SceneInstances from Instanced entities
        auto query = resource->Query<Component::SceneInstance>();
        for (const auto entity: query)
        {
            resource->registry.erase_if(entity, [](auto, const auto &storage)
            {
                return !ShouldPackWithInstance(storage);
            });
        }

        resource->registry.compact();

        return resource;
    }

    Entity::Id SceneGraph::Instantiate(std::shared_ptr<SceneResource> scene)
    {
        const auto map = Map(*scene, *this);

        for (auto& [local, current]: map)
        {
            if (const auto instance = TryGetComponent<Component::SceneInstance>(current))
            {
                CopyEntityFromSceneInstance(current, *instance);
                continue;
            }

            registry.emplace<Component::SceneInstance>(current, scene, local);
        }

        return Entity::Null; // TODO: return scene root
    }

    void SceneGraph::Replace(const SceneResource& scene)
    {
        Clear();

        Copy(scene, *this);

        auto query = Query<Component::SceneInstance>();
        for (const auto entity : query)
        {
            const auto& instance = query.GetComponent<Component::SceneInstance>(entity);
            CopyEntityFromSceneInstance(entity, instance);
        }
    }

    void SceneGraph::CopyEntityFromSceneInstance(Entity::Id entity, const Component::SceneInstance& instance)
    {
        for (auto [id, from] : instance.scene->registry.storage())
        {
            if (ShouldPackWithInstance(from))
            {
                continue;
            }

            assert(registry.storage(id) != nullptr && "Storage must not be null");

            if (auto* to = registry.storage(id); from.contains(instance.local))
            {
                to->push(entity, from.value(instance.local));
            }
        }
    }

    void SceneGraph::Update()
    {
        {
            auto view = registry.view<Component::Delete>();

            for (const auto entity : view)
            {
                registry.destroy(entity);
            }
        }

        {
            auto query = Query<Component::Transform>(Exclusion<Component::Hierarchy>);

            for (const auto entity : query)
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
