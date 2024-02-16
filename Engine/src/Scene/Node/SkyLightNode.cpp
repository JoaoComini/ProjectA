#include "SkyLightNode.hpp"

#include "Rendering/Renderer.hpp"

namespace Engine
{
    void SkyLightNode::SetCubemap(ResourceId cubemap)
    {
        this->cubemap = cubemap;
    }

    ResourceId SkyLightNode::GetCubemap() const
    {
        return cubemap;
    }

    void SkyLightNode::OnSceneAdd()
    {
        Renderer::Get().SetSkyLight({ cubemap });
    }

    void SkyLightNode::OnSceneRemove()
    {

    }
}