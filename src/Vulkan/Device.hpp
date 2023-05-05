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
        ~Device();
        static std::unique_ptr<Device> Create(PhysicalDevice physicalDevice);
        VkQueue FindQueue(QueueType type) const;
        uint32_t FindQueueIndex(QueueType type) const;

        void WaitIdle();

        SurfaceSupportDetails GetSurfaceSupportDetails() const;

        VkDevice GetHandle() const;

    private:
        VkDevice handle;
        PhysicalDevice physicalDevice;

        friend class SwapchainBuilder;
    };
}
