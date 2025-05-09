#pragma once

#include "Scene/Scene.h"

namespace Engine::Component
{
    struct SceneInstance
    {
        std::shared_ptr<SceneResource> scene;
        Entity::Id local;

        std::unordered_map<entt::id_type, std::vector<uint8_t>> overrides;
    };

    template <class Archive>
    void Save(Archive& ar, const SceneInstance& instance);

    template <class Archive>
    void Load(Archive& ar, SceneInstance& instance)
    {
        auto& manager = cereal::get_user_data<ResourceManager>(ar);

        ResourceId id{ 0 };
        ar(id);

        instance.scene = manager.template LoadResource<SceneResource>(id);

        ar(instance.local);
    }
}