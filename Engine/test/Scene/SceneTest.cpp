#include <catch2/catch_test_macros.hpp>

#include "Scene/SceneGraph.h"

#include "Fixture/Components.h"

using namespace Engine;

TEST_CASE("it should create an entity", "[Scene]")
{
    SceneGraph scene;

    auto entity = scene.CreateEntity();

    REQUIRE(entity != static_cast<Entity::Id>(Entity::Null));
}

TEST_CASE("it should iterate over all entities", "[Scene]")
{
    SceneGraph scene;

    auto a = scene.CreateEntity();
    scene.AddComponent<EmptyComponent>(a);

    auto b = scene.CreateEntity();
    scene.AddComponent<EmptyComponent>(b);

    int count = 0;
    auto query = scene.Query<EmptyComponent>();

    for (auto entity : query)
    {
        count += 1;
    }

    REQUIRE(count == 2);
}

TEST_CASE("it should not iterate over destroyed entities", "[Scene]")
{
    SceneGraph scene;

    auto a = scene.CreateEntity();
    scene.AddComponent<EmptyComponent>(a);

    auto b = scene.CreateEntity();
    scene.AddComponent<EmptyComponent>(b);

    scene.DestroyEntity(a);

    int count = 0;
    auto query = scene.Query<EmptyComponent>();

    for (auto entity : query)
    {
        count += 1;
    }

    REQUIRE(count == 1);
}

TEST_CASE("it should find the first entity", "[Scene]")
{
    SceneGraph scene;

    auto a = scene.CreateEntity();
    scene.AddComponent<FakeComponent>(a, 10, -1);

    auto query = scene.Query<FakeComponent>();

    REQUIRE(query.First() != Entity::Null);
}

TEST_CASE("it should set parent/child relationships", "[Scene]")
{
    SceneGraph scene;

    auto parent = scene.CreateEntity();
    auto child1 = scene.CreateEntity();
    auto child2 = scene.CreateEntity();

    scene.SetParent(child1, parent);
    scene.SetParent(child2, parent);

    const auto& children = scene.GetComponent<Component::Children>(parent);
    const auto& hierarchy1 = scene.GetComponent<Component::Hierarchy>(child1);
    const auto& hierarchy2 = scene.GetComponent<Component::Hierarchy>(child2);

    REQUIRE(children.size == 2);
    REQUIRE(children.first == child2);

    REQUIRE(hierarchy1.parent == parent);
    REQUIRE(hierarchy1.next == Entity::Null);
    REQUIRE(hierarchy1.prev == child2);

    REQUIRE(hierarchy2.parent == parent);
    REQUIRE(hierarchy2.next == child1);
    REQUIRE(hierarchy2.prev == Entity::Null);
}

TEST_CASE("it should unset parent/child relationships", "[Scene]")
{
    SceneGraph scene;

    auto parent = scene.CreateEntity();
    auto child1 = scene.CreateEntity();
    auto child2 = scene.CreateEntity();
    auto child3 = scene.CreateEntity();

    scene.SetParent(child1, parent);
    scene.SetParent(child2, parent);
    scene.SetParent(child3, parent);

    scene.SetParent(child2, Entity::Null);

    const auto& children = scene.GetComponent<Component::Children>(parent);
    const auto& hierarchy1 = scene.GetComponent<Component::Hierarchy>(child1);
    const auto& hierarchy3 = scene.GetComponent<Component::Hierarchy>(child3);

    REQUIRE(!scene.HasComponent<Component::Hierarchy>(child2));

    REQUIRE(children.size == 2);
    REQUIRE(children.first == child3);

    REQUIRE(hierarchy1.parent == parent);
    REQUIRE(hierarchy1.next == Entity::Null);
    REQUIRE(hierarchy1.prev == child3);

    REQUIRE(hierarchy3.parent == parent);
    REQUIRE(hierarchy3.next == child1);
    REQUIRE(hierarchy3.prev == Entity::Null);

    scene.SetParent(child1, Entity::Null);
    scene.SetParent(child3, Entity::Null);

    REQUIRE(!scene.HasComponent<Component::Hierarchy>(child1));
    REQUIRE(!scene.HasComponent<Component::Hierarchy>(child3));

    REQUIRE(!scene.HasComponent<Component::Children>(parent));
}
