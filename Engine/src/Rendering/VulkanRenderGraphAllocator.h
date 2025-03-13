#pragma once

#include "RenderGraphAllocator.h"

namespace Vulkan
{
    class Device;
}

namespace Engine
{
    class RenderAttachment;

    class VulkanRenderGraphAllocator final : public RenderGraphAllocator
    {
    public:
        explicit VulkanRenderGraphAllocator(Vulkan::Device& device);

        RenderTexture Allocate(const RenderTextureDesc& desc) override;
        void Free(RenderTexture resource, const RenderTextureDesc& desc) override;

        RenderBuffer Allocate(const RenderBufferDesc &desc) override;
        void Free(RenderBuffer resource, const RenderBufferDesc &desc) override;

    private:
        std::vector<std::unique_ptr<RenderAttachment>> attachments;
        std::unordered_map<RenderTextureDesc, std::vector<RenderAttachment*>> pool;
        
        Vulkan::Device& device;
    };
}
