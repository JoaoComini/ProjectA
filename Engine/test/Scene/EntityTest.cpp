#include <catch2/catch_test_macros.hpp>

#include <entt/entt.hpp>
#include "(Scene/Entity.h")

#include "(Fixture/Components.h")

TEST_CASE("it should add components to itself", "[Entity]")
{
    entt::registry registry;

    auto handle = registry.create();

    Engine::Entity entity(handle, &registry);

    auto& component = entity.AddComponent<FakeComponent>(10, -10);

    REQUIRE(component.x == 10);
    REQUIRE(component.y == -10);
}

TEST_CASE("it should get components", "[Entity]")
{
    entt::registry registry;

    auto handle = registry.create();

    Engine::Entity entity(handle, &registry);

    entity.AddComponent<FakeComponent>(10, -10);

    auto component = entity.GetComponent<FakeComponent>();

    REQUIRE(component.x == 10);
    REQUIRE(component.y == -10);
}

TEST_CASE("it should try to get components", "[Entity]")
{
    entt::registry registry;

    auto handle = registry.create();

    Engine::Entity entity(handle, &registry);

    entity.AddComponent<FakeComponent>(10, -10);

    SECTION("and return it if it exists")
    {
        auto component = entity.TryGetComponent<FakeComponent>();

        REQUIRE(component->x == 10);
        REQUIRE(component->y == -10);
    }

    SECTION("or return nullptr if it does not exists")
    {
        auto component = entity.TryGetComponent<OtherComponent>();

        REQUIRE(component == nullptr);
    }

}
