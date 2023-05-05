#include "PhysicalDevice.hpp"

namespace Vulkan
{
    PhysicalDevice::PhysicalDevice(VkPhysicalDevice handle, VkSurfaceKHR surface) : handle(handle), surface(surface)
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(handle, &count, nullptr);

        families.resize(count);
        vkGetPhysicalDeviceQueueFamilyProperties(handle, &count, families.data());
    }

    uint32_t PhysicalDevice::FindQueueIndex(QueueType type) const
    {
        VkQueueFlagBits flag;

        switch (type)
        {
        case Graphics:
            return FindFirstQueueIndex(VK_QUEUE_GRAPHICS_BIT);
        case Present:
            return FindPresentQueueIndex();
        }

        return Details::QUEUE_INDEX_MAX_VALUE;
    }

    uint32_t PhysicalDevice::FindFirstQueueIndex(VkQueueFlagBits flag) const
    {
        for (int i = 0; i < families.size(); i++)
        {
            if (families[i].queueFlags & flag)
            {
                return i;
            }
        }

        return Details::QUEUE_INDEX_MAX_VALUE;
    }

    uint32_t PhysicalDevice::FindPresentQueueIndex() const
    {
        for (int i = 0; i < families.size(); i++)
        {
            VkBool32 supported = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(handle, i, surface, &supported);

            if (supported)
            {
                return i;
            }
        }

        return Details::QUEUE_INDEX_MAX_VALUE;
    }

    SurfaceSupportDetails PhysicalDevice::GetSurfaceSupportDetails() const
    {
        SurfaceSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handle, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    PhysicalDevice PhysicalDevicePicker::PickBestSuitable(const Instance &instance, VkSurfaceKHR surface)
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
        bool hasGraphicsQueue = device.FindQueueIndex(QueueType::Graphics) != Details::QUEUE_INDEX_MAX_VALUE;
        bool hasPresentQueue = device.FindQueueIndex(QueueType::Present) != Details::QUEUE_INDEX_MAX_VALUE;

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