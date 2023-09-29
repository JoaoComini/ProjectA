#include "Window.hpp"

#include "WindowSurface.hpp"

namespace Engine {

    Window::Window(std::string title, int width, int height, bool resizable)
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

        handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        glfwSetWindowUserPointer(handle, this);

        glfwSetFramebufferSizeCallback(handle, [](GLFWwindow* window, int width, int height) {
            Window* pointer = (Window*)glfwGetWindowUserPointer(window);
            if (pointer->onResizeFn)
            {
                pointer->onResizeFn(width, height);
            }
        });

        glfwSetKeyCallback(handle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Window* pointer = (Window*)glfwGetWindowUserPointer(window);
            if (pointer->onKeyFn)
            {
                pointer->onKeyFn(key, action);
            }
        });

        glfwSetWindowCloseCallback(handle, [](GLFWwindow* window) {
            Window* pointer = (Window*)glfwGetWindowUserPointer(window);
            if (pointer->onCloseFn)
            {
                pointer->onCloseFn();
            }
        });

    }

    Window::~Window()
    {
        Close();
    }

    bool Window::ShouldClose()
    {
        return glfwWindowShouldClose(handle);
    }

    void Window::Close()
    {
        glfwDestroyWindow(handle);
        glfwTerminate();
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

    GLFWwindow* Window::GetHandle() const
    {
        return handle;
    }

    void Window::OnResize(std::function<void(int, int)> callback)
    {
        this->onResizeFn = callback;
    }

    void Window::OnKey(std::function<void(int, int)> callback)
    {
        this->onKeyFn = callback;
    }

    void Window::OnClose(std::function<void()> callback)
    {
        this->onCloseFn = callback;
    }

    std::unique_ptr<Vulkan::Surface> Window::CreateSurface(Vulkan::Instance& instance)
    {
        return std::make_unique<WindowSurface>(instance, handle);
    }

    FramebufferSize Window::GetFramebufferSize() const
    {
        FramebufferSize size;
        glfwGetFramebufferSize(handle, &size.width, &size.height);

        return size;
    }

    std::unique_ptr<Window> WindowBuilder::Build()
    {

        std::unique_ptr<Window> window = std::make_unique<Window>(title, width, height, true);

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

    WindowBuilder WindowBuilder::Title(std::string title)
    {
        this->title = title;
        return *this;
    }

}
