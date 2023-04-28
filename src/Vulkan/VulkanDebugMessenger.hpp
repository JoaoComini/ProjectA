#ifndef NDEBUG
#pragma once

#include <vulkan/vulkan.h>
#include <iostream>

class VulkanDebugMessenger
{
public:
    VulkanDebugMessenger(VkInstance instance);
    ~VulkanDebugMessenger() = default;
    static VkDebugUtilsMessengerCreateInfoEXT GetCreateInfo();
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
        void *userData);

    void Destroy();

private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT messenger;
};

#endif