#pragma once

#include "DebugMessenger.h"
#include "Resource.h"

namespace Vulkan
{
    class Instance : public Resource<VkInstance>
    {
    public:
        Instance();
        ~Instance() override;

    private:
#ifndef NDEBUG
        DebugMessenger messenger;
#endif

        bool CheckValidationLayerSupport();
        std::vector<const char*> GetRequiredExtensions();
    };
}
