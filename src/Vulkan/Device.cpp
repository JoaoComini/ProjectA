#include "Device.hpp"

namespace Vulkan
{
    Device::Device(const Instance &instance, const PhysicalDevice &physicalDevice) : physicalDevice(physicalDevice)
    {
        graphicsQueueFamilyIndex = physicalDevice.FindQueueIndex(QueueType::Graphics);
        presentQueueFamilyIndex = physicalDevice.FindQueueIndex(QueueType::Present);

        std::set<uint32_t> familyIndices = {graphicsQueueFamilyIndex, presentQueueFamilyIndex};

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        float queuePriority = 1.0f;
        for (uint32_t index : familyIndices)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = index;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        const std::vector<const char *> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = 0;

        if (vkCreateDevice(physicalDevice.handle, &createInfo, nullptr, &handle) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(handle, graphicsQueueFamilyIndex, 0, &graphicsQueue);
        vkGetDeviceQueue(handle, presentQueueFamilyIndex, 0, &presentQueue);

        const VmaAllocatorCreateInfo allocatorCreateInfo{
            .physicalDevice = physicalDevice.handle,
            .device = handle,
            .instance = instance.GetHandle(),
            .vulkanApiVersion = VK_API_VERSION_1_1,
        };

        if (const auto result = vmaCreateAllocator(&allocatorCreateInfo, &allocator); result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create vmaCreateAllocator!");
        }
    }

    Device::~Device()
    {
        vmaDestroyAllocator(allocator);
        vkDestroyDevice(handle, nullptr);
    }

    VkQueue Device::GetGraphicsQueue() const
    {
        return graphicsQueue;
    }

    VkQueue Device::GetPresentQueue() const
    {
        return presentQueue;
    }

    uint32_t Device::GetGraphicsQueueFamilyIndex() const
    {
        return graphicsQueueFamilyIndex;
    }

    uint32_t Device::GetPresentQueueFamilyIndex() const
    {
        return presentQueueFamilyIndex;
    }

    VmaAllocator Device::GetAllocator() const
    {
        return allocator;
    }

    void Device::WaitIdle()
    {
        vkDeviceWaitIdle(handle);
    }

    SurfaceSupportDetails Device::GetSurfaceSupportDetails() const
    {
        return physicalDevice.GetSurfaceSupportDetails();
    }

    VkDevice Device::GetHandle() const
    {
        return handle;
    }
}
