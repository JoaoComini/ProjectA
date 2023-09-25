#include "EntityManager.hpp"

#include "Components.hpp"

namespace Engine
{
    Entity EntityManager::CreateEntity()
    {
        return { registry.create(), &registry };
    }

}