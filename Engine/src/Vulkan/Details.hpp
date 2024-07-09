#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <limits>
#include <algorithm>

namespace Vulkan::Details
{
    const uint32_t QUEUE_INDEX_MAX_VALUE = 65536;

    VkSurfaceFormatKHR FindSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR FindPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D GetImageExtent(int desiredWidth, int desiredHeight, const VkSurfaceCapabilitiesKHR &capabilities);

    bool IsDepthFormat(VkFormat format);
    bool IsDepthOnlyFormat(VkFormat format);
    bool IsDepthStencilFormat(VkFormat format);
}