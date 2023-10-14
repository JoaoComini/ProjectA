#include "ModelFactory.hpp"

#include <fstream>

#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
    ResourceId ModelFactory::Create(std::filesystem::path destination, Model& model)
    {
        ResourceId id;

        std::ofstream file(destination, std::ios::out | std::ios::binary | std::ios::trunc);

        auto& root = model.GetRoot();

        WriteNode(file, root);
       
        file.close();

        return id;
    }

    void ModelFactory::WriteNode(std::ofstream& file, Node& node)
    {
        auto name = node.GetName();
        auto nameSize = name.size();
        file.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        file.write(name.data(), nameSize);

        auto& transform = node.GetTransform();
        file.write(reinterpret_cast<char*>(glm::value_ptr(transform.position)), sizeof(transform.position));
        file.write(reinterpret_cast<char*>(glm::value_ptr(transform.rotation)), sizeof(transform.rotation));
        file.write(reinterpret_cast<char*>(glm::value_ptr(transform.scale)), sizeof(transform.scale));

        auto mesh = node.GetMesh();
        file.write(reinterpret_cast<char*>(&mesh), sizeof(mesh));

        auto& children = node.GetChildren();

        auto size = children.size();
        file.write(reinterpret_cast<char*>(&size), sizeof(size));

        for (auto child : children)
        {
            WriteNode(file, *child);
        }
    }

    std::shared_ptr<Model> ModelFactory::Load(std::filesystem::path source)
    {
        auto model = std::make_shared<Model>();

        std::ifstream file(source, std::ios::in | std::ios::binary);

        std::vector<std::unique_ptr<Node>> nodes;
        ReadNodes(file, nullptr, nodes);

        model->SetRoot(*nodes.back());
        model->SetNodes(std::move(nodes));

        file.close();

        return model;
    }

    void ModelFactory::ReadNodes(std::ifstream& file, Node* parent, std::vector<std::unique_ptr<Node>>& nodes)
    {
        auto node = std::make_unique<Node>();

        size_t nameSize{};
        file.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize)); 

        std::string name(nameSize, '\0');
        file.read(name.data(), nameSize);

        node->SetName(name);

        auto& transform = node->GetTransform();
        file.read(reinterpret_cast<char*>(glm::value_ptr(transform.position)), sizeof(transform.position));
        file.read(reinterpret_cast<char*>(glm::value_ptr(transform.rotation)), sizeof(transform.rotation));
        file.read(reinterpret_cast<char*>(glm::value_ptr(transform.scale)), sizeof(transform.scale));

        ResourceId mesh{ 0 };
        file.read(reinterpret_cast<char*>(&mesh), sizeof(mesh));

        node->SetMesh(mesh);

        size_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));

        for (int i = 0; i < size; i++)
        {
            ReadNodes(file, node.get(), nodes);
        }

        if (parent)
        {
            parent->AddChild(*node);
            node->SetParent(*parent);
        }

        nodes.push_back(std::move(node));
    }
};