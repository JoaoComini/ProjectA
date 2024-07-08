#include "Pass.hpp"

#include "Rendering/Renderer.hpp"

#include "Vulkan/Caching/ResourceCache.hpp"

namespace Engine
{

    Pass::Pass(Vulkan::Device& device, std::vector<std::unique_ptr<Subpass>>&& subpasses)
        : device(device), subpasses(std::move(subpasses))
    {
        clearValues[0].color = { 0.f, 0.f, 0.f, 1.f };
        clearValues[1].depthStencil = { 0.f, 0 };
    }

    void Pass::Draw(Vulkan::CommandBuffer& commandBuffer, RenderTarget& renderTarget)
    {
        auto& attachments = renderTarget.GetAttachments();

        while (clearValues.size() < attachments.size())
        {
            clearValues.push_back({ 0.0f, 0.0f, 0.0f, 1.0f });
        }

        auto& views = renderTarget.GetViews();

        commandBuffer.BeginRendering(attachments, views, clearValues, loadStoreInfos, renderTarget.GetExtent());

        for (size_t i = 0; i < subpasses.size(); i++)
        {
            subpasses[i]->Draw(commandBuffer);
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
}