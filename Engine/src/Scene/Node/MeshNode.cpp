#include "MeshNode.hpp"

#include "Rendering/Renderer.hpp"

namespace Engine
{
    MeshNode::MeshNode() { }

    MeshNode::MeshNode(const MeshNode& node)
        : TransformNode(node)
    {
        mesh = node.mesh;
    }

    void MeshNode::OnSceneAdd()
    {
        TransformNode::OnSceneAdd();

        instance = Renderer::Get().AddMeshInstance({ mesh, GetGlobalTransform().GetMatrix() });
    }

    void MeshNode::OnSceneRemove()
    {
        Renderer::Get().RemoveMeshInstance(instance);
    }

    void MeshNode::OnTransformChanged()
    {
        Renderer::Get().UpdateMeshInstance(instance, { mesh, GetGlobalTransform().GetMatrix() });
    }

    Node* MeshNode::Clone() const
    {
        return new MeshNode(*this);
    }

    ResourceId MeshNode::GetMesh() const
    {
        return mesh;
    }

    void MeshNode::SetMesh(ResourceId mesh)
    {
        this->mesh = mesh;
    }
}
