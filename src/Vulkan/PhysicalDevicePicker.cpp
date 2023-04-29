#include "PhysicalDevicePicker.hpp"

namespace Vulkan
{
    PhysicalDevice PhysicalDevicePicker::PickBestSuitablePhysicalDevice(Instance const& instance, VkSurfaceKHR surface)
    {
        uint32_t count = 0;
        vkEnumeratePhysicalDevices(instance.GetHandle(), &count, nullptr);
        if (count == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(count);
        vkEnumeratePhysicalDevices(instance.GetHandle(), &count, devices.data());

        for (const auto &device : devices)
        {
            PhysicalDevice physicalDevice(device, surface);
            if (IsDeviceSuitable(physicalDevice))
            {
                return physicalDevice;
            }
        }

        throw std::runtime_error("failed to find a suitable GPU!");
    }

    bool PhysicalDevicePicker::IsDeviceSuitable(PhysicalDevice device)
    {
        return HasSuitableQueueFamily(device) && HasExtensionsSupport(device) && HasSwapchainSupport(device);
    }

    bool PhysicalDevicePicker::HasSuitableQueueFamily(PhysicalDevice device)
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device.handle, &count, nullptr);

        std::vector<VkQueueFamilyProperties> families(count);
        vkGetPhysicalDeviceQueueFamilyProperties(device.handle, &count, families.data());

        bool hasGraphicsQueue = Details::FindFirstQueueIndex(families, VK_QUEUE_GRAPHICS_BIT) != Details::QUEUE_INDEX_MAX_VALUE;
        bool hasPresentQueue = Details::FindPresentQueueIndex(device) != Details::QUEUE_INDEX_MAX_VALUE;

        return hasGraphicsQueue && hasPresentQueue;
    }

    bool PhysicalDevicePicker::HasExtensionsSupport(PhysicalDevice device)
    {
        uint32_t count;
        vkEnumerateDeviceExtensionProperties(device.handle, nullptr, &count, nullptr);

        std::vector<VkExtensionProperties> extensions(count);
        vkEnumerateDeviceExtensionProperties(device.handle, nullptr, &count, extensions.data());

        for (const auto &extension : extensions)
        {
            if (strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
            {
                return true;
            }
        }

        return false;
    }

    bool PhysicalDevicePicker::HasSwapchainSupport(PhysicalDevice device)
    {
        VkSurfaceCapabilitiesKHR capabilities;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.handle, device.surface, &capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device.handle, device.surface, &formatCount, nullptr);

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.handle, device.surface, &presentModeCount, nullptr);

        return formatCount > 0 && presentModeCount > 0;
    }
}
