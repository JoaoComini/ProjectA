#include "VulkanRenderGraphAllocator.hpp"

#include "Rendering/RenderAttachment.hpp"

namespace Engine
{
    VulkanRenderGraphAllocator::VulkanRenderGraphAllocator(Vulkan::Device& device)
        : device(device)
    {
    }

    RenderTexture VulkanRenderGraphAllocator::Allocate(const RenderTextureDesc& desc)
    {
        auto& vector = pool[desc];

        if (!vector.empty())
        {
            auto* attachment = vector.back();
            vector.pop_back();
            return { attachment };
        }

        VkImageUsageFlags usage{ 0 };

        if (bool(desc.usage & RenderTextureUsage::Sampled))
        {
            usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }

        if (bool(desc.usage & RenderTextureUsage::RenderTarget))
        {
            if (bool(desc.format & RenderTextureFormat::Depth))
            {
                usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            }
            else
            {
                usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            }
        }

        VkFormat format{ VK_FORMAT_UNDEFINED };

        switch (desc.format)
        {
        case RenderTextureFormat::Depth:
            format = VK_FORMAT_D16_UNORM;
            break;
        case RenderTextureFormat::HDR:
            format = VK_FORMAT_R16G16B16A16_SFLOAT;
            break;
        case RenderTextureFormat::Linear:
            format = VK_FORMAT_B8G8R8A8_UNORM;
            break;
        case RenderTextureFormat::sRGB:
            format = VK_FORMAT_B8G8R8A8_SRGB;
            break;
        }

        auto attachment = RenderAttachment::Builder(device)
            .Extent({ desc.width, desc.height })
            .Usage(usage)
            .Format(format)
            .Build();

        attachments.push_back(std::move(attachment));

        return { attachments.back().get() };
    }

    void VulkanRenderGraphAllocator::Release(const RenderTextureDesc& desc, RenderTexture resource)
    {
        assert(pool.contains(desc));

        auto& vector = pool[desc];

        vector.push_back(static_cast<RenderAttachment*>(resource.image));
    }
}