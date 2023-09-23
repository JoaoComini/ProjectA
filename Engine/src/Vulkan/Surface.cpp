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

} // namespace Vulkan
