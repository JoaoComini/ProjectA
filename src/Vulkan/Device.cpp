#include "Device.hpp"

namespace Vulkan
{
    Device Device::Create(PhysicalDevice physicalDevice)
    {
        uint32_t graphicsIndex = physicalDevice.FindQueueIndex(QueueType::Graphics);
        uint32_t presentIndex = physicalDevice.FindQueueIndex(QueueType::Present);

        std::set<uint32_t> familyIndices = {graphicsIndex, presentIndex};

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

        Device device;
        device.physicalDevice = physicalDevice;
        if (vkCreateDevice(physicalDevice.handle, &createInfo, nullptr, &device.handle) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        return device;
    }

    VkQueue Device::FindQueue(QueueType type) const
    {
        VkQueue queue;
        uint32_t index = physicalDevice.FindQueueIndex(type);

        vkGetDeviceQueue(handle, index, 0, &queue);

        return queue;
    }

    uint32_t Device::FindQueueIndex(QueueType type) const
    {
        return physicalDevice.FindQueueIndex(type);
    }

    VkDevice Device::GetHandle() const
    {
        return handle;
    }
}
