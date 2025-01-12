#include "Pass.hpp"

#include "Rendering/Renderer.hpp"
#include "Vulkan/Caching/ResourceCache.hpp"

namespace Engine
{
    Pass::Pass(RenderContext& renderContext, Vulkan::ShaderSource&& vertexSource, Vulkan::ShaderSource&& fragmentSource)
        : renderContext(renderContext), vertexShader(vertexSource), fragmentShader(fragmentSource)
    {
    }

    void Pass::Execute(Vulkan::CommandBuffer& commandBuffer, RenderTarget& renderTarget)
    {
        std::vector<VkRenderingAttachmentInfo> colorInfos;

        Vulkan::PipelineRenderingState renderingState{};
        Vulkan::MultisampleState multisampleState{};

        int colorAttachmentCount = renderTarget.GetColorAttachmentCount();

        for (int i = 0; i < colorAttachmentCount; i++)
        {
            auto& attachment = renderTarget.GetColorAttachment(i);

            auto loadStore = attachment.GetLoadStoreInfo();

            VkRenderingAttachmentInfo info{
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = attachment.GetView().GetHandle(),
                .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .loadOp = loadStore.loadOp,
                .storeOp = loadStore.storeOp,
                .clearValue = attachment.GetClearValue(),
            };

            if (auto& resolve = attachment.GetResolve())
            {
                info.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
                info.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                info.resolveImageView = resolve->GetView().GetHandle();
            }

            colorInfos.push_back(std::move(info));
            renderingState.colorAttachmentFormats.push_back(attachment.GetFormat());
            multisampleState.rasterizationSamples = attachment.GetSampleCount();
        }

        VkRenderingInfo renderingInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = {
                .offset = { 0, 0 },
                .extent = renderTarget.GetExtent(),
            },
            .layerCount = 1,
            .colorAttachmentCount = static_cast<uint32_t>(colorInfos.size()),
            .pColorAttachments = colorInfos.data(),
        };

        std::array<VkRenderingAttachmentInfo, 1> depthInfos{};

        if (renderTarget.HasDepthAttachment())
        {
            auto& depthAttachment = renderTarget.GetDepthAttachment();

            auto loadStore = depthAttachment.GetLoadStoreInfo();

            depthInfos[0] = {
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = depthAttachment.GetView().GetHandle(),
                .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .loadOp = loadStore.loadOp,
                .storeOp = loadStore.storeOp,
                .clearValue = depthAttachment.GetClearValue(),
            };

            renderingInfo.pDepthAttachment = depthInfos.data();
            renderingState.depthAttachmentFormat = depthAttachment.GetFormat();
        }

        commandBuffer.BeginRendering(renderingInfo);

        commandBuffer.SetPipelineRenderingState(renderingState);
        commandBuffer.SetMultisampleState(multisampleState);

        Draw(commandBuffer);
    }

    void Pass::BindBuffer(const Vulkan::Buffer& buffer, uint32_t offset, uint32_t size, uint32_t set, uint32_t binding, uint32_t arrayElement)
    {
        bufferBindings[set][binding][arrayElement] = { buffer.GetHandle(), offset, size };
    }

    void Pass::BindImage(const Vulkan::ImageView& imageView, const Vulkan::Sampler& sampler, uint32_t set, uint32_t binding, uint32_t arrayElement)
    {
        imageBindings[set][binding][arrayElement] = { sampler.GetHandle(), imageView.GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
    }

    void Pass::FlushDescriptorSet(Vulkan::CommandBuffer& commandBuffer, Vulkan::PipelineLayout& pipelineLayout, uint32_t set)
    {
        auto& frame = renderContext.GetCurrentFrame();

        auto& descritorSetLayout = pipelineLayout.GetDescriptorSetLayout(set);

        auto descriptorSet = frame.RequestDescriptorSet(descritorSetLayout, bufferBindings[set], imageBindings[set]);

        commandBuffer.BindDescriptorSet(descriptorSet);
    }

    RenderContext& Pass::GetRenderContext()
    {
        return renderContext;
    }

    Vulkan::PipelineLayout& Pass::GetPipelineLayout(const std::vector<Vulkan::ShaderModule*>& shaders)
    {
        return renderContext.GetDevice().GetResourceCache().RequestPipelineLayout(shaders);
    }

    const Vulkan::ShaderSource& Pass::GetVertexShader() const
    {
        return vertexShader;
    }

    const Vulkan::ShaderSource& Pass::GetFragmentShader() const
    {
        return fragmentShader;
    }
}