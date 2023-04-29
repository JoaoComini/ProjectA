#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Vulkan
{
    struct PhysicalDevice
    {
        PhysicalDevice();
        PhysicalDevice(VkPhysicalDevice handle, VkSurfaceKHR surface);
        
        VkPhysicalDevice handle;
        VkSurfaceKHR surface;
        std::vector<VkQueueFamilyProperties> families;
    };
}
