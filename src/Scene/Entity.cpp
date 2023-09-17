#include "Entity.hpp"


namespace Scene
{
    Entity::Entity(entt::entity handle, entt::registry* registry) : handle(handle), registry(registry)
    {

    }
}