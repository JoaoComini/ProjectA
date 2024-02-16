#include "Node.hpp"

#include "Scene/Scene.hpp"

namespace Engine
{
    Node::Node() {}

    Node::Node(const Node& node)
    {
        name = node.name;

        for (auto child : node.children)
        {
            AddChild(child->Clone());
        }
    }

    void Node::SetName(const std::string& name)
    {
        this->name = name;
    }

    std::string Node::GetName() const
    {
        return name;
    }

    void Node::AddChild(Node* node)
    {
        assert(node != nullptr);
        assert(node != this);

        children.emplace(node);

        node->parent = this;

        if (scene)
        {
            node->SetScene(scene);
        }
    }
    
    void Node::RemoveChild(Node* node)
    {
        assert(node->parent == this);

        bool erased = children.erase(node);
        assert(erased);

        node->parent = nullptr;
        node->SetScene(nullptr);
    }

    const std::set<Node*> Node::GetChildren() const
    {
        return children;
    }

    void Node::SetScene(Scene* scene)
    {
        if (this->scene)
        {
            OnSceneRemove();
        }

        this->scene = scene;

        if (this->scene)
        {
            OnSceneAdd();
        }

        for (auto child : children)
        {
            child->SetScene(scene);
        }
    }

    Node* Node::GetParent()
    {
        return parent;
    }

    Node* Node::Clone() const
    {
        return new Node(*this);
    }

}