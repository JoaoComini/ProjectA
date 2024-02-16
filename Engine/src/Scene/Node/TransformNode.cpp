#include "TransformNode.hpp"


namespace Engine
{
    TransformNode::TransformNode() { }

    TransformNode::TransformNode(const TransformNode& node)
        : Node(node)
    {
        local = node.local;

        scale = local.basis.GetScale();
        euler = local.basis.GetEulerNormalized();
    }

    void TransformNode::SetPosition(const glm::vec3& position)
    {
        local.origin = position;

        PropagateGlobalChanges();
    }

    glm::vec3 TransformNode::GetPosition() const
    {
        return local.origin;
    }

    void TransformNode::SetRotation(const glm::vec3& euler)
    {
        this->euler = euler;

        SetDirtyBit(LOCAL);

        PropagateGlobalChanges();
    }

    glm::vec3 TransformNode::GetRotation() const
    {
        return euler;
    }

    void TransformNode::SetScale(const glm::vec3& scale)
    {
        this->scale = scale;

        SetDirtyBit(LOCAL);

        PropagateGlobalChanges();
    }

    glm::vec3 TransformNode::GetScale() const
    {
        return scale;
    }

    void TransformNode::SetTransform(const Transform& transform)
    {
        local = transform;

        scale = local.basis.GetScale();
        euler = local.basis.GetEulerNormalized();
    }

    Transform TransformNode::GetTransform()
    {
        if (IsDirtyBit(LOCAL))
        {
            UpdateLocalTransform();
        }

        return local;
    }

    void TransformNode::SetGlobalTransform(const Transform& transform)
    {
        auto xform = parent
            ? parent->GetGlobalTransform().AffineInverse() * transform
            : transform;

        SetTransform(xform);
    }

    Transform TransformNode::GetGlobalTransform()
    {
        if (!IsDirtyBit(GLOBAL))
        {
            return global;
        }

        if (IsDirtyBit(LOCAL))
        {
            UpdateLocalTransform();
        }

        global = local;

        if (parent)
        {
            global = parent->GetGlobalTransform() * global;
        }

        ClearDirtyBit(GLOBAL);

        return global;
    }

    void TransformNode::OnSceneAdd()
    {
        if (GetParent())
        {
            if (parent = dynamic_cast<TransformNode*>(GetParent()))
            {
                parent->children.insert(this);
            }
        }

        SetDirtyBit(GLOBAL);
    }

    void TransformNode::OnSceneRemove()
    {
        if (parent)
        {
            parent->children.erase(this);
        }

        parent = nullptr;
    }

    void TransformNode::UpdateLocalTransform()
    {
        local.basis.SetEulerAndScale(euler, scale);

        ClearDirtyBit(LOCAL);
    }

    void TransformNode::PropagateGlobalChanges()
    {     
        SetDirtyBit(GLOBAL);

        for (auto child : children)
        {
            child->PropagateGlobalChanges();
        }

        OnTransformChanged();
    }

    void TransformNode::SetDirtyBit(uint8_t bit)
    {
        dirty |= bit;
    }

    void TransformNode::ClearDirtyBit(uint8_t bit)
    {
        dirty &= ~bit;
    }

    bool TransformNode::IsDirtyBit(uint8_t bit) const
    {
        return dirty & bit;
    }

    Node* TransformNode::Clone() const
    {
        return new TransformNode(*this);
    }
}