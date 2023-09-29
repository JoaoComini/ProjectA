#include <catch2/catch_test_macros.hpp>

#include "Scene/Scene.hpp"

struct Component
{
    int x;
    int y;
};


TEST_CASE("it should create an entity", "[Scene]")
{

    Engine::Scene scene;

    auto entity = scene.CreateEntity();
    entity.AddComponent<Component>();
}
