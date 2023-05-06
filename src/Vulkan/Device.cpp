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

        VkDeviceCreateInfo deviceCreateinfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledLayerCount = 0,
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
            .pEnabledFeatures = &deviceFeatures,
        };

        if (vkCreateDevice(physicalDevice.handle, &deviceCreateinfo, nullptr, &handle) != VK_SUCCESS)
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

        VkCommandPoolCreateInfo poolCreateInfo{};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

        if (vkCreateCommandPool(handle, &poolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    Device::~Device()
    {
        vkDestroyCommandPool(handle, commandPool, nullptr);
        vmaDestroyAllocator(allocator);
        vkDestroyDevice(handle, nullptr);
    }

    void Device::WaitIdle()
    {
        vkDeviceWaitIdle(handle);
    }

    void Device::CopyBuffer(VkBuffer src, VkBuffer dest, uint32_t size)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(handle, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copy{};
        copy.srcOffset = 0; // Optional
        copy.dstOffset = 0; // Optional
        copy.size = size;
        vkCmdCopyBuffer(commandBuffer, src, dest, 1, &copy);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);
        
        vkFreeCommandBuffers(handle, commandPool, 1, &commandBuffer);
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

    VkCommandPool Device::GetCommandPool() const
    {
        return commandPool;
    }

    VmaAllocator Device::GetAllocator() const
    {
        return allocator;
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
