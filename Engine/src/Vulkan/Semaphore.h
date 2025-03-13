#pragma once

#include "Resource.h"
#include "Device.h"

namespace Vulkan
{
    class Semaphore : public Resource<VkSemaphore>
    {

    public:
        Semaphore(const Device &device);
        ~Semaphore();

    private:
        const Device &device;
    };

} // namespace Vulkan
