#include <catch2/catch_test_macros.hpp>

#include "Scene/Scene.hpp"

#include "Fixture/Components.hpp"

using namespace Engine;

TEST_CASE("it should create an entity", "[Scene]")
{
    Scene scene;

    auto entity = scene.CreateEntity();

    REQUIRE(entity);
}

TEST_CASE("it should iterate over all entities", "[Scene]")
{
    Scene scene;

    auto a = scene.CreateEntity();
    a.AddComponent<EmptyComponent>();

    auto b = scene.CreateEntity();
    b.AddComponent<EmptyComponent>();

    int count = 0;
    scene.ForEachEntity<EmptyComponent>([&count](Entity entity) {
        count += 1;
    });

    REQUIRE(count == 2);
}

TEST_CASE("it should find the first entity", "[Scene]")
{
    Scene scene;

    auto a = scene.CreateEntity();
    a.AddComponent<FakeComponent>(10, -1);

    auto b = scene.CreateEntity();
    b.AddComponent<FakeComponent>(5, 8);

    auto [entity, found] = scene.FindFirstEntity<FakeComponent>();
    REQUIRE(found);

    SECTION("that matches the predicate")
    {
        auto[entity, found] = scene.FindFirstEntity<FakeComponent>([](Entity e) {
            auto c = e.GetComponent<FakeComponent>();

            return c.x == 10;
        });

        REQUIRE(found);
        REQUIRE(entity == a);
    }
}
