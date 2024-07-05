#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "DebugMessenger.hpp"
#include "Resource.hpp"

namespace Vulkan
{
    struct Instance : public Resource<VkInstance>
    {
    public:
        Instance();
        ~Instance();

    private:
#ifndef NDEBUG
        DebugMessenger messenger;
#endif

        bool CheckValidationLayerSupport();
        std::vector<const char*> GetRequiredExtensions();
    };
}
