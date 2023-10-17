#include <catch2/catch_test_macros.hpp>

#include "Resource/Factory/PrefabFactory.hpp"

using namespace Engine;

TEST_CASE("it should create and retrieve a model", "[PrefabFactory]")
{
    PrefabFactory factory;

    auto root = std::make_unique<Node>();
    root->SetName("root");
    root->SetMesh(1);

    auto first = std::make_unique<Node>();
    first->SetName("first");
    first->SetMesh(2);

    auto second = std::make_unique<Node>();
    second->SetName("second");
    second->SetMesh(3);
    
    root->AddChild(*first);
    root->AddChild(*second);

    Prefab model;
    model.SetRoot(*root);
    
    std::vector<std::unique_ptr<Node>> nodes;
    nodes.push_back(std::move(first));
    nodes.push_back(std::move(second));
    nodes.push_back(std::move(root));

    model.SetNodes(std::move(nodes));

    factory.Create("test.prefab", model);

    auto loaded = factory.Load("test.prefab");

    REQUIRE(loaded->GetRoot().GetName() == "root");
    REQUIRE(loaded->GetRoot().GetMesh() == ResourceId{ 1 });
    REQUIRE(loaded->GetRoot().GetChildren().size() == 2);
}