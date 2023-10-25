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

    const std::vector<uint32_t>& Subpass::GetInputAttachments() const
    {
        return inputAttachments;
    }

    void Subpass::SetInputAttachments(std::vector<uint32_t> input)
    {
        inputAttachments = input;
    }

    const std::vector<uint32_t>& Subpass::GetOutputAttachments() const
    {
        return outputAttachments;
    }

    void Subpass::SetOutputAttachments(std::vector<uint32_t> output)
    {
        outputAttachments = output;
    }

    const std::vector<uint32_t>& Subpass::GetColorResolveAttachments() const
    {
        return colorResolveAttachments;
    }

    void Subpass::SetColorResolveAttachments(std::vector<uint32_t> colorResolve)
    {
        colorResolveAttachments = colorResolve;
    }

    bool Subpass::IsDepthStencilDisabled() const
    {
        return disableDepthStencil;
    }

    void Subpass::EnableDepthStencil()
    {
        disableDepthStencil = false;
    }
};