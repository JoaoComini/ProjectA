#include "Subpass.hpp"


namespace Engine
{
    Subpass::Subpass(Vulkan::ShaderSource&& vertexSource, Vulkan::ShaderSource&& fragmentSource)
        : vertexShader(vertexSource), fragmentShader(fragmentSource)
    {
    }

    const Vulkan::ShaderSource& Subpass::GetVertexShader() const
    {
        return vertexShader;
    }

    const Vulkan::ShaderSource& Subpass::GetFragmentShader() const
    {
        return fragmentShader;
    }
};