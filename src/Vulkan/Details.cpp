#include "Details.hpp"

namespace Vulkan::Details
{
    uint32_t FindFirstQueueIndex(std::vector<VkQueueFamilyProperties> families, VkQueueFlags flags)
    {
        for (int i = 0; i < families.size(); i++)
        {
            if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                return i;
            }
        }

        return QUEUE_INDEX_MAX_VALUE;
    }

    uint32_t FindPresentQueueIndex(PhysicalDevice device)
    {
        for (int i = 0; i < device.families.size(); i++)
        {
            VkBool32 supported = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device.handle, i, device.surface, &supported);

            if (supported)
            {
                return i;
            }
        }

        return QUEUE_INDEX_MAX_VALUE;
    }

}