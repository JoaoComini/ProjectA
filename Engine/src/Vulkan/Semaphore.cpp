#include "Semaphore.hpp"

namespace Vulkan
{
    Semaphore::Semaphore(const Device &device) : device(device)
    {
        VkSemaphoreCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        vkCreateSemaphore(device.GetHandle(), &createInfo, nullptr, &handle);
    }

    Semaphore::~Semaphore()
    {
        vkDestroySemaphore(device.GetHandle(), handle, nullptr);
    }
    
} // namespace Vulkan
