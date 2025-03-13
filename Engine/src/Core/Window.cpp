#include "Window.h"

#include "WindowSurface.h"

namespace Engine {

    KeyCode GlfwKeyToKeyCode(int key);

    Window::Window(std::string title, int width, int height, bool resizable)
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

        handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        glfwSetWindowUserPointer(handle, this);

        glfwSetFramebufferSizeCallback(handle, [](GLFWwindow* window, int width, int height) {
            Window* pointer = (Window*)glfwGetWindowUserPointer(window);

            pointer->callbacks.onResize(width, height);
        });

        glfwSetKeyCallback(handle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Window* pointer = (Window*)glfwGetWindowUserPointer(window);

            pointer->callbacks.onInputEvent(KeyInputEvent{
                GlfwKeyToKeyCode(key),
                static_cast<KeyAction>(action)
            });
        });

        glfwSetMouseButtonCallback(handle, [](GLFWwindow* window, int button, int action, int mods) {
            Window* pointer = (Window*)glfwGetWindowUserPointer(window);

            pointer->callbacks.onInputEvent(MouseButtonInputEvent{
                static_cast<MouseButton>(button),
                static_cast<MouseButtonAction>(action)
            });
        });

        glfwSetCursorPosCallback(handle, [](GLFWwindow* window, double x, double y) {
            Window* pointer = (Window*)glfwGetWindowUserPointer(window);

            pointer->callbacks.onInputEvent(MouseMoveInputEvent{
                static_cast<float>(x),
                static_cast<float>(y) 
            });
        });

        glfwSetWindowCloseCallback(handle, [](GLFWwindow* window) {
            Window* pointer = (Window*)glfwGetWindowUserPointer(window);

            pointer->callbacks.onClose();
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

    void* Window::GetHandle() const
    {
        return handle;
    }

    void Window::OnResize(std::function<void(int, int)> callback)
    {
        callbacks.onResize = callback;
    }

    void Window::OnClose(std::function<void()> callback)
    {
        callbacks.onClose = callback;
    }

    void Window::OnInputEvent(std::function<void(const InputEvent&)> func)
    {
        callbacks.onInputEvent = func;
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

    KeyCode GlfwKeyToKeyCode(int key)
    {
        switch (key)
        {
        case GLFW_KEY_TAB:         return KeyCode::Tab;
        case GLFW_KEY_LEFT_SHIFT:  return KeyCode::LeftShift;
        case GLFW_KEY_LEFT_CONTROL:return KeyCode::LeftControl;
        case GLFW_KEY_SPACE:       return KeyCode::Space;
        case GLFW_KEY_ENTER:       return KeyCode::Enter;
        case GLFW_KEY_ESCAPE:      return KeyCode::Escape;
        case GLFW_KEY_BACKSPACE:   return KeyCode::Backspace;
        case GLFW_KEY_DELETE:      return KeyCode::Delete;

        case GLFW_KEY_A: return KeyCode::A;
        case GLFW_KEY_B: return KeyCode::B;
        case GLFW_KEY_C: return KeyCode::C;
        case GLFW_KEY_D: return KeyCode::D;
        case GLFW_KEY_E: return KeyCode::E;
        case GLFW_KEY_F: return KeyCode::F;
        case GLFW_KEY_G: return KeyCode::G;
        case GLFW_KEY_H: return KeyCode::H;
        case GLFW_KEY_I: return KeyCode::I;
        case GLFW_KEY_J: return KeyCode::J;
        case GLFW_KEY_K: return KeyCode::K;
        case GLFW_KEY_L: return KeyCode::L;
        case GLFW_KEY_M: return KeyCode::M;
        case GLFW_KEY_N: return KeyCode::N;
        case GLFW_KEY_O: return KeyCode::O;
        case GLFW_KEY_P: return KeyCode::P;
        case GLFW_KEY_Q: return KeyCode::Q;
        case GLFW_KEY_R: return KeyCode::R;
        case GLFW_KEY_S: return KeyCode::S;
        case GLFW_KEY_T: return KeyCode::T;
        case GLFW_KEY_U: return KeyCode::U;
        case GLFW_KEY_V: return KeyCode::V;
        case GLFW_KEY_W: return KeyCode::W;
        case GLFW_KEY_X: return KeyCode::X;
        case GLFW_KEY_Y: return KeyCode::Y;
        case GLFW_KEY_Z: return KeyCode::Z;

        case GLFW_KEY_F1:  return KeyCode::F1;
        case GLFW_KEY_F2:  return KeyCode::F2;
        case GLFW_KEY_F3:  return KeyCode::F3;
        case GLFW_KEY_F4:  return KeyCode::F4;
        case GLFW_KEY_F5:  return KeyCode::F5;
        case GLFW_KEY_F6:  return KeyCode::F6;
        case GLFW_KEY_F7:  return KeyCode::F7;
        case GLFW_KEY_F8:  return KeyCode::F8;
        case GLFW_KEY_F9:  return KeyCode::F9;
        case GLFW_KEY_F10: return KeyCode::F10;
        case GLFW_KEY_F11: return KeyCode::F11;
        case GLFW_KEY_F12: return KeyCode::F12;

        default: return KeyCode::Unknown;
        }
    }

}
