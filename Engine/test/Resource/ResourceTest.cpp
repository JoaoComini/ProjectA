#include <catch2/catch_test_macros.hpp>

#include "Resource/Resource.hpp"

using namespace Engine;

TEST_CASE("it should convert any ResourceType enum to string", "[ResourceType]")
{
    REQUIRE(ResourceTypeToString(ResourceType::Mesh) == "Mesh");
    REQUIRE(ResourceTypeToString(ResourceType::Texture) == "Texture");
    REQUIRE(ResourceTypeToString(ResourceType::None) == "None");
}

TEST_CASE("it should a string to a ResourceType enum", "[ResourceType]")
{
    REQUIRE(StringToResourceType("Mesh") == ResourceType::Mesh);
    REQUIRE(StringToResourceType("Texture") == ResourceType::Texture);
    REQUIRE(StringToResourceType("None") == ResourceType::None);
    REQUIRE(StringToResourceType("Invalid") == ResourceType::None);
}