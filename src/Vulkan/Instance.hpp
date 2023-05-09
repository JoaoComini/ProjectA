#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>
#include <string.h>

#include "DebugMessenger.hpp"
#include "Resource.hpp"

namespace Vulkan
{
    struct Instance: public Resource<VkInstance>
    {
    public:
        ~Instance();

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
