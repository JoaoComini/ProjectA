#include "Window.hpp"

Window::Window(std::string name, int width, int height, void *userPointer, bool resizable) : userPointer(userPointer)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    handle = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(handle, this);

    auto lambda = [](GLFWwindow *window, int width, int height)
    {
        auto pointer = static_cast<Window *>(glfwGetWindowUserPointer(window));
        pointer->framebufferSizeCallback(pointer, FramebufferSize{height, width});
    };

    glfwSetFramebufferSizeCallback(handle, lambda);
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

FramebufferSize Window::GetFramebufferSize()
{
    FramebufferSize size;
    glfwGetFramebufferSize(handle, &size.width, &size.height);

    return size;
}

void Window::SetFramebufferSizeCallback(void (*callback)(Window *window, FramebufferSize size))
{
    this->framebufferSizeCallback = callback;
}

std::unique_ptr<Window> WindowBuilder::Build()
{

    std::unique_ptr<Window> window = std::make_unique<Window>("Vulkan", width, height, userPointer, false);

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

WindowBuilder WindowBuilder::UserPointer(void *userPointer)
{
    this->userPointer = userPointer;
    return *this;
}
