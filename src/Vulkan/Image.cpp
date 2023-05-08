#include "Image.hpp"

namespace Vulkan
{
    Image::Image(const Device &device, uint32_t width, uint32_t height) : device(device)
    {
        VkExtent3D extent = {
            width,
            height,
            1,
        };

        VkImageCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_D32_SFLOAT,
            .extent = extent,
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        };

        VmaAllocationCreateInfo allocationCreateInfo{
            .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO,
            .priority = 1.0f,
        };

        vmaCreateImage(device.GetAllocator(), &createInfo, &allocationCreateInfo, &handle, &allocation, nullptr);

        VkImageViewCreateInfo viewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .image = handle,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_D32_SFLOAT,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        vkCreateImageView(device.GetHandle(), &viewCreateInfo, nullptr, &imageView);
    }

    Image::~Image()
    {
        vkDestroyImageView(device.GetHandle(), imageView, nullptr);
        vmaDestroyImage(device.GetAllocator(), handle, allocation);
    }

    VkImageView Image::GetImageView() const
    {
        return imageView;
    }

} // namespace Vulkan
