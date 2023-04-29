#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>
#include <string.h>

#include "DebugMessenger.hpp"

namespace Vulkan
{
    class Instance
    {
    public:
        ~Instance();

        void Create();
        VkInstance GetHandle() const;

    private:
        VkInstance handle;
        VkPhysicalDevice physicalDevice;

        bool CheckValidationLayerSupport();
        std::vector<const char *> GetRequiredExtensions();

#ifndef NDEBUG
        std::unique_ptr<DebugMessenger> messenger;
#endif
    };
}
