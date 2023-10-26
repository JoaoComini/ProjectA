#include "Subpass.hpp"

#include "Rendering/Renderer.hpp"

namespace Engine
{
    Subpass::Subpass(Vulkan::ShaderSource&& vertexSource, Vulkan::ShaderSource&& fragmentSource)
        : vertexShader(vertexSource), fragmentShader(fragmentSource)
    {
    }

    void Subpass::BindBuffer(const Vulkan::Buffer& buffer, uint32_t offset, uint32_t size, uint32_t set, uint32_t binding, uint32_t arrayElement)
    {
        bufferBindings[set][binding][arrayElement] = { buffer.GetHandle(), offset, size };
    }

    void Subpass::BindImage(const Vulkan::ImageView& imageView, const Vulkan::Sampler& sampler, uint32_t set, uint32_t binding, uint32_t arrayElement)
    {
        imageBindings[set][binding][arrayElement] = { sampler.GetHandle(), imageView.GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
    }

    void Subpass::FlushDescriptorSet(Vulkan::CommandBuffer& commandBuffer, uint32_t set)
    {
        auto& frame = Renderer::Get().GetCurrentFrame();

        auto& descritorSetLayout = pipelineLayout->GetDescriptorSetLayout(set);

        auto descriptorSet = frame.RequestDescriptorSet(descritorSetLayout, bufferBindings[set], imageBindings[set]);

        commandBuffer.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, *pipelineLayout, set, descriptorSet);
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