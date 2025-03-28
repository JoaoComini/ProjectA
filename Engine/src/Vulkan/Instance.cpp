#include "Instance.h"

#include <GLFW/glfw3.h>

namespace Vulkan
{
    Instance::Instance()
    {
#ifndef NDEBUG
        if (!CheckValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }
#endif

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "ProjectA";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = GetRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

#ifndef NDEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = DebugMessenger::GetCreateInfo();
        const char *layer = "VK_LAYER_KHRONOS_validation";
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = &layer;
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
#else
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
#endif
        if (vkCreateInstance(&createInfo, nullptr, &handle) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }

#ifndef NDEBUG
        messenger = DebugMessenger(handle);
#endif
    }

    bool Instance::CheckValidationLayerSupport()
    {
        uint32_t count;
        vkEnumerateInstanceLayerProperties(&count, nullptr);

        std::vector<VkLayerProperties> availableLayers(count);
        vkEnumerateInstanceLayerProperties(&count, availableLayers.data());

        for (const auto &layerProperties : availableLayers)
        {
            if (std::strcmp("VK_LAYER_KHRONOS_validation", layerProperties.layerName) == 0)
            {
                return true;
            }
        }

        return false;
    }

    std::vector<const char *> Instance::GetRequiredExtensions()
    {
        uint32_t count = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&count);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + count);

        extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

#ifndef NDEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        return extensions;
    }

    Instance::~Instance()
    {
#ifndef NDEBUG
        messenger.Destroy();
#endif
        vkDestroyInstance(handle, nullptr);
    }
}