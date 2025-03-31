#pragma once

#include "Scene/Components.h"

#include "SceneInstance.h"

namespace Engine::Component
{
    template <typename...>
    struct GroupT
    {
        explicit constexpr GroupT() = default;
    };

    template <typename... Components>
    struct Group : GroupT<Components...>{};

    inline constexpr auto Serializable = Group<
        Transform,
        Name,
        SceneInstance,
        Children,
        Hierarchy,
        MeshRender,
        Camera,
        DirectionalLight,
        PointLight,
        // SkyLight, TODO: fix skylight
        Script,
        PhysicsBody,
        BoxShape,
        SphereShape
    >{};
}