#include "PhysicalDevice.hpp"

#include <stdexcept>

namespace Vulkan
{
    PhysicalDevice::PhysicalDevice(VkPhysicalDevice handle, const Surface& surface) : handle(handle), surface(surface)
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(handle, &count, nullptr);

        families.resize(count);
        vkGetPhysicalDeviceQueueFamilyProperties(handle, &count, families.data());
    }

    uint32_t PhysicalDevice::FindQueueIndex(Queue::Type type) const
    {
        VkQueueFlagBits flag;

        switch (type)
        {
        case Queue::Type::GRAPHICS:
            return FindFirstQueueIndex(VK_QUEUE_GRAPHICS_BIT);
        case Queue::Type::PRESENT:
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
            vkGetPhysicalDeviceSurfaceSupportKHR(handle, i, surface.GetHandle(), &supported);

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

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handle, surface.GetHandle(), &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface.GetHandle(), &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface.GetHandle(), &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface.GetHandle(), &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface.GetHandle(), &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkPhysicalDeviceProperties PhysicalDevice::GetProperties() const
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(handle, &properties);

        return properties;
    }

    VkFormat PhysicalDevice::GetSupportedDepthFormat(bool DepthOnly) const
    {
        std::vector<VkFormat> candidates = {
                VK_FORMAT_D32_SFLOAT,
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT,
                VK_FORMAT_D16_UNORM_S8_UINT,
                VK_FORMAT_D16_UNORM
        };

        for (auto& candidate : candidates)
        {
            if (DepthOnly && !Details::IsDepthOnlyFormat(candidate))
            {
                continue;
            }

            VkFormatProperties properties{};
            vkGetPhysicalDeviceFormatProperties(handle, candidate, &properties);

            if (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                return candidate;
            }
        }

        return VK_FORMAT_UNDEFINED;
    }

    VkPhysicalDevice PhysicalDevice::GetHandle() const
    {
        return handle;
    }

    std::unique_ptr<PhysicalDevice> PhysicalDevicePicker::PickBestSuitable(const Instance &instance, const Surface& surface)
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
                return std::make_unique<PhysicalDevice>(std::move(physicalDevice));
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
        bool hasGraphicsQueue = device.FindQueueIndex(Queue::Type::GRAPHICS) != Details::QUEUE_INDEX_MAX_VALUE;
        bool hasPresentQueue = device.FindQueueIndex(Queue::Type::PRESENT) != Details::QUEUE_INDEX_MAX_VALUE;

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

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.handle, device.surface.GetHandle(), &capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device.handle, device.surface.GetHandle(), &formatCount, nullptr);

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.handle, device.surface.GetHandle(), &presentModeCount, nullptr);

        return formatCount > 0 && presentModeCount > 0;
    }
}