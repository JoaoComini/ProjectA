#include "Pass.hpp"

#include "Renderer.hpp"

namespace Engine
{

    Pass::Pass(Vulkan::Device& device, std::vector<std::unique_ptr<Subpass>>&& subpasses)
        : device(device), subpasses(std::move(subpasses))
    {
        clearValues[0].color = { 0.f, 0.f, 0.f, 1.f };
        clearValues[1].depthStencil = { 0.f, 0 };
    }

    void Pass::Prepare(const RenderTarget& target)
    {
        std::vector<Vulkan::SubpassInfo> subpassInfos{};
        for (auto& subpass : subpasses)
        {
            Vulkan::SubpassInfo info{};
            info.inputAttachments = subpass->GetInputAttachments();
            info.outputAttachments = subpass->GetOutputAttachments();
            info.colorResolveAttachments = subpass->GetColorResolveAttachments();
            info.disableDepthStencilAttachment = subpass->IsDepthStencilDisabled();

            subpassInfos.push_back(std::move(info));
        }

        auto& attachments = target.GetAttachments();

        while (clearValues.size() < attachments.size())
        {
            clearValues.push_back({ 0.0f, 0.0f, 0.0f, 1.0f });
        }

        renderPass = std::make_unique<Vulkan::RenderPass>(device, attachments, loadStoreInfos, subpassInfos);

        for (auto& subpass : subpasses)
        {
            subpass->Prepare(*renderPass);
        }
    }

    void Pass::Draw(Vulkan::CommandBuffer& commandBuffer, RenderTarget& renderTarget)
    {
        auto& framebuffer = renderTarget.RequestFramebuffer(*renderPass);

        commandBuffer.BeginRenderPass(*renderPass, framebuffer, clearValues, renderTarget.GetExtent());

        for (size_t i = 0; i < subpasses.size(); i++)
        {
            subpasses[i]->Draw(commandBuffer);

            if (i < subpasses.size() - 1)
            {
                commandBuffer.NextSubpass();
            }
        }
    }

    void Pass::SetLoadStoreInfos(std::vector<Vulkan::LoadStoreInfo> loadStoreInfos)
    {
        this->loadStoreInfos = loadStoreInfos;
    }

    void Pass::SetClearValues(std::vector<VkClearValue> clearValues)
    {
        this->clearValues = clearValues;
    }

    Vulkan::RenderPass& Pass::GetRenderPass() const
    {
        return *renderPass;
    }
}