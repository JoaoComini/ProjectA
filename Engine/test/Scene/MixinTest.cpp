#include <catch2/catch_test_macros.hpp>

#include "Scene/Entity.hpp"

template<typename, typename, typename = void>
struct has_on_construct : std::false_type {};

template<typename Entity, typename Type>
struct has_on_construct<Entity, Type, std::void_t<decltype(&entt::storage_type_t<Type>::on_construct)>> : std::true_type {};

template<typename Entity, typename Type>
inline constexpr auto has_on_construct_v = has_on_construct<Entity, Type>::value;

struct Listener
{
    int count = 0;

    void F(Engine::Entity entity)
    {
        count += 1;
    }
};

TEST_CASE("it should listen to construct events", "[Mixin]")
{
    static_assert(has_on_construct_v<Engine::Entity, int>);
    static_assert(has_on_construct_v<Engine::Entity, char>);

    entt::registry registry;

    const entt::entity entity[1u]{ registry.create() };

    Listener listener;

    registry.on_construct<int>().connect<&Listener::F>(listener);
    registry.on_construct<char>().connect<&Listener::F>(listener);

    registry.emplace<int>(entity[0], 42);
    registry.emplace<char>(entity[0], 43);
    registry.emplace<long>(entity[0], 43l);

    REQUIRE(listener.count == 2);
}
