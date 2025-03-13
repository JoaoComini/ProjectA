#include "WindowSurface.h"

namespace Engine {

    WindowSurface::WindowSurface(Vulkan::Instance& instance, GLFWwindow* window) : Vulkan::Surface(instance)
    {
        if (glfwCreateWindowSurface(instance.GetHandle(), window, nullptr, &handle) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

}
