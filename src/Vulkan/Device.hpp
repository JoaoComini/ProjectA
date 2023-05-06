#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

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
        Device(const Instance &instance, const PhysicalDevice &physicalDevice);
        ~Device();

        void CopyBuffer(VkBuffer src, VkBuffer dest, uint32_t size);
        void WaitIdle();

        VkQueue GetPresentQueue() const;
        VkQueue GetGraphicsQueue() const;

        uint32_t GetGraphicsQueueFamilyIndex() const;
        uint32_t GetPresentQueueFamilyIndex() const;

        VkCommandPool GetCommandPool() const;
        VmaAllocator GetAllocator() const;

        SurfaceSupportDetails GetSurfaceSupportDetails() const;

        VkDevice GetHandle() const;

    private:
        VkDevice handle;
        VkCommandPool commandPool;
        VmaAllocator allocator;

        VkQueue presentQueue;
        VkQueue graphicsQueue;

        uint32_t graphicsQueueFamilyIndex;
        uint32_t presentQueueFamilyIndex;

        const PhysicalDevice &physicalDevice;

        friend class SwapchainBuilder;
    };
}
