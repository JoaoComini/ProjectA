#include <catch2/catch_test_macros.hpp>

#include "Scene/EntityManager.hpp"

struct Component
{
    int x;
    int y;
};


TEST_CASE("it should create an entity", "[Scene]")
{

    Engine::EntityManager manager;

    auto entity = manager.CreateEntity();
    entity.AddComponent<Component>();
}
