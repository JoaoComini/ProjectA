#ifndef NDEBUG
#pragma once

namespace Vulkan
{
    class DebugMessenger
    {
    public:
        DebugMessenger(const VkInstance &instance);
        DebugMessenger() = default;
        static VkDebugUtilsMessengerCreateInfoEXT GetCreateInfo();

        void Destroy();

    private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT type,
            const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
            void *userData);
    };
}

#endif