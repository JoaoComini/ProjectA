#include <catch2/catch_test_macros.hpp>

#include <entt/entt.hpp>

#include "Scene/Entity.hpp"

struct Component
{
    int x;
    int y;
};


TEST_CASE("it should add components to itself", "[Entity]")
{
    entt::registry registry;

    auto handle = registry.create();

    Scene::Entity entity(handle, &registry);

    auto comp = entity.AddComponent<Component>(1, 2);

    REQUIRE(comp.x == 1);
    REQUIRE(comp.y == 2);
}
