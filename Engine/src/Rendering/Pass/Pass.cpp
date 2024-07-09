#include "Pass.hpp"

#include <array>

#include "Rendering/Renderer.hpp"
#include "Vulkan/Caching/ResourceCache.hpp"

namespace Engine
{

    Pass::Pass(Vulkan::Device& device, std::vector<std::unique_ptr<Subpass>>&& subpasses)
        : device(device), subpasses(std::move(subpasses))
    {
    }

    void Pass::Draw(Vulkan::CommandBuffer& commandBuffer, RenderTarget& renderTarget)
    {
        auto& colorAttachments = renderTarget.GetColorAttachments();

        std::vector<VkRenderingAttachmentInfo> colorInfos;

        Vulkan::PipelineRenderingState renderingState{};
        Vulkan::MultisampleState multisampleState{};

        for (auto& attachment : colorAttachments)
        {
            auto loadStore = attachment->GetLoadStoreInfo();

            VkRenderingAttachmentInfo info{
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = attachment->GetView().GetHandle(),
                .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .loadOp = loadStore.loadOp,
                .storeOp = loadStore.storeOp,
                .clearValue = attachment->GetClearValue(),
            };

            if (auto& resolve = attachment->GetResolve())
            {
                info.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
                info.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                info.resolveImageView = resolve->GetView().GetHandle();
            }

            colorInfos.push_back(std::move(info));
            renderingState.colorAttachmentFormats.push_back(attachment->GetFormat());
            multisampleState.rasterizationSamples = attachment->GetSampleCount();
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

        if (auto& depthAttachment = renderTarget.GetDepthAttachment())
        {
            auto loadStore = depthAttachment->GetLoadStoreInfo();

            depthInfos[0] = {
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = depthAttachment->GetView().GetHandle(),
                .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .loadOp = loadStore.loadOp,
                .storeOp = loadStore.storeOp,
                .clearValue = depthAttachment->GetClearValue(),
            };

            renderingInfo.pDepthAttachment = depthInfos.data();
            renderingState.depthAttachmentFormat = depthAttachment->GetFormat();
        }

        commandBuffer.BeginRendering(renderingInfo);

        commandBuffer.SetPipelineRenderingState(renderingState);
        commandBuffer.SetMultisampleState(multisampleState);

        for (size_t i = 0; i < subpasses.size(); i++)
        {
            subpasses[i]->Draw(commandBuffer);
        }
    }
}