#include "ScriptEntity.h"


namespace Engine
{
    ScriptEntity::ScriptEntity(SceneGraph& scene, Entity::Id entity)
        : scene(scene), entity(entity)
    {
    }

    Entity::Id ScriptEntity::GetId() const
    {
        return entity;
    }
}