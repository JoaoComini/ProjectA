#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>
#include <string.h>

#include "VulkanDebugMessenger.hpp"

class VulkanInstance
{
public:
    VulkanInstance();
    ~VulkanInstance();

    VkInstance GetHandle();
private:
    VkInstance handle;
    bool CheckValidationLayerSupport();
    std::vector<const char *> GetRequiredExtensions();

#ifndef NDEBUG
    std::unique_ptr<VulkanDebugMessenger> messenger;
#endif
};
