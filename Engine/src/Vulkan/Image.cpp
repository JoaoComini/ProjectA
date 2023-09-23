#include "Image.hpp"

#include "Device.hpp"

namespace Vulkan
{
    Image::Image(const Device &device, VkImageUsageFlags usage, VkFormat format, VkExtent3D extent, VkSampleCountFlagBits samples, uint32_t mipLevels)
        : device(device), format(format), extent(extent), mipLevels(mipLevels)
    {

        VkImageCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent = extent,
            .mipLevels = mipLevels,
            .arrayLayers = 1,
            .samples = samples,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        VmaAllocationCreateInfo allocationCreateInfo{
            .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO,
            .priority = 1.0f,
        };

        vmaCreateImage(device.GetAllocator(), &createInfo, &allocationCreateInfo, &handle, &allocation, nullptr);
    }

    VkExtent3D Image::GetExtent() const
    {
        return extent;
    }

    uint32_t Image::GetMipLevels() const
    {
        return mipLevels;
    }

    Image::Image(const Device& device, VkImage handle, VkFormat format) : device(device), Resource(handle), format(format)
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

    VkFormat Image::GetFormat() const
    {
        return format;
    }

} // namespace Vulkan
