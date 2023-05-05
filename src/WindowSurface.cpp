#include "WindowSurface.hpp"

WindowSurface::WindowSurface(Vulkan::Instance &instance, GLFWwindow *window) : Vulkan::Surface(instance), window(window)
{
    if (glfwCreateWindowSurface(instance.GetHandle(), window, nullptr, &handle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}
