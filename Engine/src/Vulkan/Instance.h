#pragma once

#include "DebugMessenger.h"
#include "Resource.h"

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
