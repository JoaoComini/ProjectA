#include <catch2/catch_test_macros.hpp>

#include "Scene/Scene.hpp"

#include "Fixture/Components.hpp"

TEST_CASE("it should create an entity", "[Scene]")
{
    Engine::Scene scene;

    auto entity = scene.CreateEntity();

    REQUIRE(entity);
}

TEST_CASE("it should iterate over all entities", "[Scene]")
{
    Engine::Scene scene;

    auto a = scene.CreateEntity();
    a.AddComponent<EmptyComponent>();

    auto b = scene.CreateEntity();
    b.AddComponent<EmptyComponent>();

    int count = 0;
    scene.ForEachEntity<EmptyComponent>([&count](auto entity) {
        count += 1;
    });

    REQUIRE(count == 2);
}

TEST_CASE("it should find the first entity", "[Scene]")
{
    Engine::Scene scene;

    auto a = scene.CreateEntity();
    a.AddComponent<Component>(10, -1);

    auto b = scene.CreateEntity();
    b.AddComponent<Component>(5, 8);

    auto [entity, found] = scene.FindFirstEntity<Component>();
    REQUIRE(found);

    SECTION("that matches the predicate")
    {
        auto[entity, found] = scene.FindFirstEntity<Component>([](auto e) {
            auto c = e.GetComponent<Component>();

            return c.x == 10;
        });

        REQUIRE(found);
        REQUIRE(entity == a);
    }
}
