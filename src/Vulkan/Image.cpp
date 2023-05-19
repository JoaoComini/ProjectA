#include "Image.hpp"

#include "Device.hpp"

namespace Vulkan
{
    Image::Image(const Device &device, ImageUsage usage, ImageFormat format, uint32_t width, uint32_t height) : device(device)
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
            .format = static_cast<VkFormat>(format),
            .extent = extent,
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = static_cast<VkImageUsageFlags>(usage),
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        VmaAllocationCreateInfo allocationCreateInfo{
            .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO,
            .priority = 1.0f,
        };

        vmaCreateImage(device.GetAllocator(), &createInfo, &allocationCreateInfo, &handle, &allocation, nullptr);
    }

    Image::Image(const Device& device, VkImage handle) : device(device), Resource(handle)
    {
    }

    Image::~Image()
    {
        if (handle == VK_NULL_HANDLE || allocation == VK_NULL_HANDLE)
        {
            return;
        }

        vmaDestroyImage(device.GetAllocator(), handle, allocation);
    }

} // namespace Vulkan
