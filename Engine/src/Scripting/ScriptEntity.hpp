#pragma once

#include "Scene/Scene.hpp"

namespace Engine
{
    class ScriptEntity
    {
    public:
        ScriptEntity(Scene& scene, Entity::Id entity);

        template<typename T>
        decltype(auto) TryGetComponent() const
        {
            return scene.TryGetComponent<T>(entity);
        }

        Entity::Id GetId() const;

    private:
        Scene& scene;
        Entity::Id entity{ Entity::Null };
    };
}