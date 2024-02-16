#pragma once

#include "Resource/Resource.hpp"

#include "Node.hpp"

namespace Engine
{
    class SkyLightNode : public Node
    {

    REFLECT(SkyLightNode)

    public:

        void SetCubemap(ResourceId cubemap);
        ResourceId GetCubemap() const;

        virtual void OnSceneAdd() override;
        virtual void OnSceneRemove() override;

    private:
        ResourceId cubemap{ 0 };
    };
}