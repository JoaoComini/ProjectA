#pragma once

#include <vulkan/vulkan.h>

#include "Instance.hpp"

namespace Vulkan
{
    class Surface
    {
    public:
        Surface(Instance &instance);
        virtual ~Surface();

        VkSurfaceKHR GetHandle() const;

    protected:
        Instance &instance;
        VkSurfaceKHR handle;
    };

} // namespace Vulkan
