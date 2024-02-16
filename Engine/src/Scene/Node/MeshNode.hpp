#pragma once

#include "Resource/Resource.hpp"

#include "TransformNode.hpp"

namespace Engine
{
    class MeshNode : public TransformNode
    {

    REFLECT(MeshNode)

    public:
        MeshNode();
        MeshNode(const MeshNode&);

        ResourceId GetMesh() const;
        void SetMesh(ResourceId mesh);

        virtual void OnSceneAdd() override;
        virtual void OnSceneRemove() override;

        virtual void OnTransformChanged() override;

        virtual Node* Clone() const override;

    private:
        ResourceId mesh{ 0 };
        Uuid instance{ 0 };
    };
}