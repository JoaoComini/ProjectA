#pragma once

#include "RenderGraphAllocator.hpp"

namespace Vulkan
{
    class Device;
}

namespace Engine
{
    class RenderAttachment;

    class VulkanRenderGraphAllocator : public RenderGraphAllocator
    {
    public:
        VulkanRenderGraphAllocator(Vulkan::Device& device);

        RenderTexture Allocate(const RenderTextureDesc& desc) override;
        void Release(const RenderTextureDesc& desc, RenderTexture resource) override;

    private:
        std::vector<std::unique_ptr<RenderAttachment>> attachments;
        std::unordered_map<RenderTextureDesc, std::vector<RenderAttachment*>> pool;
        
        Vulkan::Device& device;
    };
}
