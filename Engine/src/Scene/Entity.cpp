#include "Entity.hpp"


namespace Engine
{
    Entity::Entity(entt::entity handle, entt::registry* registry) : handle(handle), registry(registry)
    {

    }
}