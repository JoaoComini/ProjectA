#include "Window.hpp"

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(handle);
}

void Window::Update()
{
    glfwPollEvents();
}

void Window::Destroy()
{
    glfwDestroyWindow(handle);
    glfwTerminate();
}

void Window::GetFrameBufferSize(int &width, int &height)
{
    glfwGetFramebufferSize(handle, &width, &height);
}

void Window::CreateSurface(Vulkan::Instance instance)
{
    if (glfwCreateWindowSurface(instance.handle, handle, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

VkSurfaceKHR Window::GetSurface() const
{
    return surface;
}

Window WindowBuilder::Build()
{
    Window window;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window.handle = glfwCreateWindow(width, heigth, "Vulkan", nullptr, nullptr);

    return window;
}

WindowBuilder WindowBuilder::Width(uint32_t width)
{
    this->width = width;
    return *this;
}

WindowBuilder WindowBuilder::Height(uint32_t height)
{
    this->heigth = height;
    return *this;
}
