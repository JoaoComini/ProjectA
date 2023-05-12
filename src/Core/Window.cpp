#include "Window.hpp"

Window::Window(std::string name, int width, int height, bool resizable)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    handle = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(handle, this);
}

Window::~Window()
{
    glfwDestroyWindow(handle);
    glfwTerminate();
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(handle);
}

void Window::Update()
{
    glfwPollEvents();
}

void Window::WaitForFocus()
{
    auto size = GetFramebufferSize();
    while (size.width == 0 || size.height == 0)
    {
        size = GetFramebufferSize();
        glfwWaitEvents();
    }
}

GLFWwindow *Window::GetHandle() const
{
    return handle;
}

FramebufferSize Window::GetFramebufferSize() const
{
    FramebufferSize size;
    glfwGetFramebufferSize(handle, &size.width, &size.height);

    return size;
}


std::unique_ptr<Window> WindowBuilder::Build()
{

    std::unique_ptr<Window> window = std::make_unique<Window>("Vulkan", width, height, false);

    return window;
}

WindowBuilder WindowBuilder::Width(uint32_t width)
{
    this->width = width;
    return *this;
}

WindowBuilder WindowBuilder::Height(uint32_t height)
{
    this->height = height;
    return *this;
}
