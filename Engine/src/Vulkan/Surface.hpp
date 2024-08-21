#pragma once

#include "Instance.hpp"
#include "Resource.hpp"

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
