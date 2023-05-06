#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Device.hpp"

namespace Vulkan
{
    class Buffer
    {        
    public:
        Buffer(const Device &device, void *data, uint32_t size);
        ~Buffer();

        VkBuffer GetHandle() const;

    private:
        VkBuffer handle;
        VmaAllocation allocation;
        VmaAllocationInfo allocationInfo;

        const Device &device;
    };
    

} // namespace Vulkan
