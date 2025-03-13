#pragma once

#include "Instance.h"
#include "Resource.h"

namespace Vulkan
{
    class Surface : public Resource<VkSurfaceKHR>
    {
    public:
        Surface(Instance &instance);
        virtual ~Surface();


    protected:
        Instance &instance;
    };

} // namespace Vulkan
