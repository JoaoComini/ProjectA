#include <catch2/catch_test_macros.hpp>

#include "Scene/EntityManager.hpp"

struct Component
{
    int x;
    int y;
};


TEST_CASE("it should create an entity", "[Scene]")
{

    Scene::EntityManager manager;

    auto entity = manager.CreateEntity();
    entity.AddComponent<Component>();

    manager.ForEachEntity<Component>([](auto& entity, auto& component) {
        component.x = 10;
    });
}
