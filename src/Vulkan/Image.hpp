#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Device.hpp"

namespace Vulkan
{
    class Image
    {
    public:
        Image(const Device &device, uint32_t width, uint32_t height);
        ~Image();

        VkImageView GetImageView() const;

    private:
        VkImage handle;
        VkImageView imageView;
        VmaAllocation allocation;

        const Device &device;
    };

} // namespace Vulkan
