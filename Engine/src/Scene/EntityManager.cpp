#include "EntityManager.hpp"

#include "Components.hpp"

namespace Scene
{
    Entity EntityManager::CreateEntity()
    {
        return { registry.create(), &registry };
    }

}