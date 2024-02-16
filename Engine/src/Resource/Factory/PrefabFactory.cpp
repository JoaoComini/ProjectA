#include "PrefabFactory.hpp"

#include "Common/FileSystem.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "Scene/Node/TransformNode.hpp"
#include "Scene/Node/MeshNode.hpp"
#include "Scene/Node/SkyLightNode.hpp"

namespace Engine
{
    void PrefabFactory::Create(std::filesystem::path destination, Prefab& model)
    {
        flatbuffers::FlatBufferBuilder builder(128);

        auto root = WriteNode(builder, model.GetRoot());

        flatbuffers::PrefabBuilder prefabBuilder(builder);
        prefabBuilder.add_root(root);

        auto offset = prefabBuilder.Finish();
        builder.Finish(offset);

        uint8_t* buffer = builder.GetBufferPointer();
        size_t size = builder.GetSize();

        FileSystem::WriteFile(destination, { buffer, buffer + size });
    }

    flatbuffers::Offset<flatbuffers::Node> PrefabFactory::WriteNode(flatbuffers::FlatBufferBuilder& builder, Node* node)
    {
        std::vector<flatbuffers::Offset<flatbuffers::Node>> childrenVector;
        for (auto child : node->GetChildren())
        {
            auto offset = WriteNode(builder, child);
            childrenVector.push_back(offset);
        }

        auto name = builder.CreateString(node->GetName());
        auto class_ = builder.CreateString(node->GetClass());

        auto children = builder.CreateVector(childrenVector);

        flatbuffers::NodeBuilder nodeBuilder(builder);
        nodeBuilder.add_name(name);
        nodeBuilder.add_class_(class_);

        if (node->GetClass() == TransformNode::GetStaticClass() || node->GetClass() == MeshNode::GetStaticClass())
        {
            auto serialized = dynamic_cast<TransformNode*>(node);

            auto position = serialized->GetPosition();
            auto rotation = serialized->GetRotation();
            auto scale = serialized->GetScale();

            nodeBuilder.add_transform(new flatbuffers::Transform(
                std::span<const float, 3>(glm::value_ptr(position), 3),
                std::span<const float, 3>(glm::value_ptr(rotation), 3),
                std::span<const float, 3>(glm::value_ptr(scale), 3)
            ));
        }
        
        if (node->GetClass() == MeshNode::GetStaticClass())
        {
            nodeBuilder.add_mesh(dynamic_cast<MeshNode*>(node)->GetMesh());
        }

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

        model->SetRoot(nodes.back().get());
        model->SetNodes(std::move(nodes));

        return model;
    }

    void PrefabFactory::ReadNodes(flatbuffers::NodeT& buffer, Node* parent, std::vector<std::unique_ptr<Node>>& nodes)
    {
        std::unique_ptr<Node> node = nullptr;

        if (buffer.class_ == Node::GetStaticClass())
        {
            node = std::make_unique<Node>();
        }
        else if (buffer.class_ == TransformNode::GetStaticClass())
        {
            auto serialized = std::make_unique<TransformNode>();

            Transform transform{};

            transform.origin = glm::make_vec3(buffer.transform->position()->data());
            transform.basis.SetEulerAndScale(
                glm::make_vec3(buffer.transform->rotation()->data()),
                glm::make_vec3(buffer.transform->scale()->data())
            );

            serialized->SetTransform(transform);

            node = std::move(serialized);
        }
        else if (buffer.class_ == MeshNode::GetStaticClass())
        {
            auto serialized = std::make_unique<MeshNode>();

            Transform transform{};

            transform.origin = glm::make_vec3(buffer.transform->position()->data());
            transform.basis.SetEulerAndScale(
                glm::make_vec3(buffer.transform->rotation()->data()),
                glm::make_vec3(buffer.transform->scale()->data())
            );

            serialized->SetTransform(transform);

            serialized->SetMesh(buffer.mesh);

            node = std::move(serialized);
        }

        node->SetName(buffer.name);

        for (auto& child : buffer.children)
        {
            ReadNodes(*child, node.get(), nodes);
        }

        if (parent)
        {
            parent->AddChild(node.get());
        }

        nodes.push_back(std::move(node));
    }
};