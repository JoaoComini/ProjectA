#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <stdexcept>
#include <string.h>

#include "Details.hpp"
#include "PhysicalDevice.hpp"
#include "Instance.hpp"

namespace Vulkan
{
    class PhysicalDevicePicker
    {
    public:
        virtual ~PhysicalDevicePicker() = default;

        static PhysicalDevice PickBestSuitablePhysicalDevice(Instance const& instance, VkSurfaceKHR surface);

    private:
        static bool IsDeviceSuitable(PhysicalDevice device);
        static bool HasSuitableQueueFamily(PhysicalDevice device);
        static bool HasExtensionsSupport(PhysicalDevice device);
        static bool HasSwapchainSupport(PhysicalDevice device);
    };
}
