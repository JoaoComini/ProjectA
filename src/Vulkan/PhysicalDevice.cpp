#include "PhysicalDevice.hpp"

namespace Vulkan
{
    PhysicalDevice::PhysicalDevice() {}

    PhysicalDevice::PhysicalDevice(VkPhysicalDevice handle, VkSurfaceKHR surface) : handle(handle), surface(surface)
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(handle, &count, nullptr);

        families.resize(count);
        vkGetPhysicalDeviceQueueFamilyProperties(handle, &count, families.data());
    }
}