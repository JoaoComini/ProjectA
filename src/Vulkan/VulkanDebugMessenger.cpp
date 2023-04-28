#ifndef NDEBUG

#include "VulkanDebugMessenger.hpp"

VulkanDebugMessenger::VulkanDebugMessenger(VkInstance instance) : instance(instance)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = GetCreateInfo();

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func == nullptr)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }

    func(instance, &createInfo, nullptr, &messenger);
}

VkDebugUtilsMessengerCreateInfoEXT VulkanDebugMessenger::GetCreateInfo()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = VulkanDebugMessenger::DebugCallback;
    return createInfo;
}

VkBool32 VulkanDebugMessenger::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
    void *userData)
{
    std::cerr << "validation layer: " << callbackData->pMessage << std::endl;

    return VK_FALSE;
}

void VulkanDebugMessenger::Destroy()
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, messenger, nullptr);
    }
}

#endif