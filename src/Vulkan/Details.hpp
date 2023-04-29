#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "PhysicalDevice.hpp"

namespace Vulkan::Details
{
    const uint32_t QUEUE_INDEX_MAX_VALUE = 65536;

    uint32_t FindFirstQueueIndex(std::vector<VkQueueFamilyProperties> families, VkQueueFlags flags);
    uint32_t FindPresentQueueIndex(PhysicalDevice device);
}