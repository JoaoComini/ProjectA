#include <catch2/catch_test_macros.hpp>

#include "Scene/Manager.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Components.hpp"

TEST_CASE("it should create an entity", "[Scene]")
{
    Scene::Manager manager;

    Scene::Entity entity = manager.CreateEntity();
}
