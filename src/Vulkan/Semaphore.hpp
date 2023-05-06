#pragma once

#include <vulkan/vulkan.h>

#include "Device.hpp"

namespace Vulkan
{
    class Semaphore
    {

    public:
        Semaphore(const Device &device);
        ~Semaphore();

        VkSemaphore GetHandle() const;

    private:
        VkSemaphore handle;
        const Device &device;
    };

} // namespace Vulkan
