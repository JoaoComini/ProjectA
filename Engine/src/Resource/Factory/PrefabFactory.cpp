#include "PrefabFactory.hpp"

#include "Common/FileSystem.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
    ResourceId PrefabFactory::Create(std::filesystem::path destination, Prefab& model)
    {
        ResourceId id;

        flatbuffers::FlatBufferBuilder builder(128);

        auto root = WriteNode(builder, model.GetRoot());

        flatbuffers::PrefabBuilder prefabBuilder(builder);
        prefabBuilder.add_root(root);

        auto offset = prefabBuilder.Finish();
        builder.Finish(offset);

        uint8_t* buffer = builder.GetBufferPointer();
        size_t size = builder.GetSize();

        FileSystem::WriteFile(destination, { buffer, buffer + size });
      
        return id;
    }

    flatbuffers::Offset<flatbuffers::Node> PrefabFactory::WriteNode(flatbuffers::FlatBufferBuilder& builder, Node& node)
    {
        std::vector<flatbuffers::Offset<flatbuffers::Node>> childrenVector;
        for (auto child : node.GetChildren())
        {
            auto offset = WriteNode(builder, *child);
            childrenVector.push_back(offset);
        }

        auto name = builder.CreateString(node.GetName());
        auto children = builder.CreateVector(childrenVector);

        flatbuffers::NodeBuilder nodeBuilder(builder);
        nodeBuilder.add_name(name);
        nodeBuilder.add_mesh(node.GetMesh());

        auto& transform = node.GetTransform();
        nodeBuilder.add_transform(new flatbuffers::Transform(
            std::span<const float, 3>(glm::value_ptr(transform.position), 3),
            std::span<const float, 4>(glm::value_ptr(transform.rotation), 4),
            std::span<const float, 3>(glm::value_ptr(transform.scale), 3)
        ));

        nodeBuilder.add_children(children);

        return nodeBuilder.Finish();
    }

    std::shared_ptr<Prefab> PrefabFactory::Load(std::filesystem::path source)
    {
        auto file = FileSystem::ReadFile(source);

        auto model = std::make_shared<Prefab>();

        flatbuffers::PrefabT buffer;
        flatbuffers::GetPrefab(file.c_str())->UnPackTo(&buffer);
    
        std::vector<std::unique_ptr<Node>> nodes;
        ReadNodes(*buffer.root, nullptr, nodes);

        model->SetRoot(*nodes.back());
        model->SetNodes(std::move(nodes));

        return model;
    }

    void PrefabFactory::ReadNodes(flatbuffers::NodeT& buffer, Node* parent, std::vector<std::unique_ptr<Node>>& nodes)
    {
        auto node = std::make_unique<Node>();

        node->SetName(buffer.name);

        auto& transform = node->GetTransform();
        transform.position = glm::make_vec3(buffer.transform->position()->data());
        transform.rotation = glm::make_quat(buffer.transform->rotation()->data());
        transform.scale = glm::make_vec3(buffer.transform->scale()->data());

        node->SetMesh(buffer.mesh);

        for (auto& child : buffer.children)
        {
            ReadNodes(*child, node.get(), nodes);
        }

        if (parent)
        {
            parent->AddChild(*node);
        }

        nodes.push_back(std::move(node));
    }
};