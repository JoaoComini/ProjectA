#include "SceneResource.h"

#include "SceneGraph.h"

namespace Engine
{

    template<typename... Args>
    void CheckForOverrides(Entity::Id entity, SceneResource& resource, Component::SceneInstance& instance, Component::GroupT<Args...>)
    {
        ([&]()
        {
            if (! resource.HasComponent<Args>(entity))
            {
                registry.erase<Args>(entity);
                return;
            }


        }(), ...);
    }

    void SceneResource::Pack(const SceneGraph &graph)
    {
        Copy(graph, *this);

        auto query = Query<Component::SceneInstance>();
        for (const auto entity: query)
        {
            auto& instance = query.GetComponent<Component::SceneInstance>(entity);
            CheckForOverrides(entity, registry, instance, Component::Overridable);
        }

        registry.compact();
    }

    ResourceType SceneResource::GetType() const
    {
        return ResourceType::Scene;
    }
};