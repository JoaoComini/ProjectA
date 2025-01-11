#include "ScriptEntity.hpp"


namespace Engine
{
    ScriptEntity::ScriptEntity(Scene& scene, Entity::Id entity)
        : scene(scene), entity(entity)
    {
    }

    Entity::Id ScriptEntity::GetId() const
    {
        return entity;
    }
}