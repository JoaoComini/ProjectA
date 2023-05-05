#include "Surface.hpp"

namespace Vulkan
{

    Surface::Surface(Instance &instance) : instance(instance)
    {
    }

    Surface::~Surface()
    {
        vkDestroySurfaceKHR(instance.GetHandle(), handle, nullptr);
    }

    VkSurfaceKHR Surface::GetHandle() const
    {
        return handle;
    }

} // namespace Vulkan
