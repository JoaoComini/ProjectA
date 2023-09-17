#include "Manager.hpp"

#include "Entity.hpp"

namespace Scene
{
    Entity Manager::CreateEntity()
    {
        return { registry.create(), &registry };
    }

}