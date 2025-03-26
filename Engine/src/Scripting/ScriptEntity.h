#pragma once

#include "Scene/SceneGraph.h"

namespace Engine
{
    class ScriptEntity
    {
    public:
        ScriptEntity(SceneGraph& scene, Entity::Id entity);

        template<typename T>
        decltype(auto) TryGetComponent() const
        {
            return scene.TryGetComponent<T>(entity);
        }

        Entity::Id GetId() const;

    private:
        SceneGraph& scene;
        Entity::Id entity{ Entity::Null };
    };
}