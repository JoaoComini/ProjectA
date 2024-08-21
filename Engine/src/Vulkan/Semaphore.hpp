#pragma once

#include "Resource.hpp"
#include "Device.hpp"

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
