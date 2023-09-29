#include "Scene.hpp"

#include "Components.hpp"

namespace Engine
{
    Entity Scene::CreateEntity()
    {
        return { registry.create(), &registry };
    }

}