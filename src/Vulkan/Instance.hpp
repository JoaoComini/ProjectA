#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>
#include <string.h>

#include "DebugMessenger.hpp"

namespace Vulkan
{
    struct Instance
    {
    public:
        ~Instance();
        VkInstance GetHandle() const;
        VkInstance handle = VK_NULL_HANDLE;

    private:
#ifndef NDEBUG
        DebugMessenger messenger;
#endif

    friend class InstanceBuilder;
    };

    class InstanceBuilder
    {
    public:
        std::unique_ptr<Instance> Build();
    private:
        bool CheckValidationLayerSupport();
        std::vector<const char *> GetRequiredExtensions();
    };
}
