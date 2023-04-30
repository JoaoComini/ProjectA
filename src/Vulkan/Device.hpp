#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <set>
#include <stdexcept>

#include "PhysicalDevice.hpp"
#include "Details.hpp"
#include "Queue.hpp"

namespace Vulkan
{    
    class Device
    {
    public:
        static Device Create(PhysicalDevice physicalDevice);
        VkQueue FindQueue(QueueType type) const;
        uint32_t FindQueueIndex(QueueType type) const;

        VkDevice GetHandle() const;
    private:
        VkDevice handle;
        PhysicalDevice physicalDevice;
    };
}
