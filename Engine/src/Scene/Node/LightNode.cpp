#include "LightNode.hpp"

#include "Rendering/Renderer.hpp"

namespace Engine
{
    LightNode::LightNode()
    {
    }

    LightNode::LightNode(const LightNode& node)
        : TransformNode(node)
    {
    }

    void LightNode::SetIntensity(float intensity)
    {
        this->intensity = intensity;
    }

    void LightNode::SetColor(const glm::vec3& color)
    {
        this->color = color;
    }

    float LightNode::GetIntensity() const
    {
        return intensity;
    }

    glm::vec3 LightNode::GetColor() const
    {
        return color;
    }

    // ----------- Directional Light ---------------

    void DirectionalLightNode::OnSceneAdd()
    {
        LightNode::OnSceneAdd();

        instance = Renderer::Get().AddDirectionalLightInstance({ GetGlobalTransform(), GetColor(), GetIntensity() });
    }

    void DirectionalLightNode::OnSceneRemove()
    {
        Renderer::Get().RemoveDirectionalLightInstance(instance);
    }

    void DirectionalLightNode::OnTransformChanged()
    {
        Renderer::Get().UpdateDirectionalLightInstance(instance, { GetGlobalTransform(), GetColor(), GetIntensity() });
    }

    // --------------- PointLight ------------------

    void PointLightNode::OnSceneAdd()
    {
        LightNode::OnSceneAdd();

        instance = Renderer::Get().AddPointLightInstance({ GetGlobalTransform(), GetColor(), GetRange() });
    }

    void PointLightNode::OnSceneRemove()
    {
        Renderer::Get().RemovePointLightInstance(instance);
    }

    void PointLightNode::OnTransformChanged()
    {
        Renderer::Get().UpdatePointLightInstance(instance, { GetGlobalTransform(), GetColor(), GetRange() });
    }

    void PointLightNode::SetRange(float range)
    {
        this->range = range;
    }

    float PointLightNode::GetRange() const
    {
        return range;
    }
}