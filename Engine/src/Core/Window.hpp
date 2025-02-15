#pragma once

#include <GLFW/glfw3.h>

#include "Vulkan/Instance.hpp"
#include "Vulkan/Surface.hpp"

#include "InputEvent.hpp"

namespace Engine {

    struct FramebufferSize
    {
        int height;
        int width;
    };

    enum class WindowCursor
    {
        Unknown = -1,
        Arrow,
        TextInput,
        ResizeAll,
        ResizeNS,
        ResizeEW,
        ResizeNESW,
        ResizeNWSE,
        Hand,
        NotAllowed
    };

    struct WindowCallbacks
    {
        std::function<void(int, int)> onResize{ nullptr };
        std::function<void()> onClose{ nullptr };
        std::function<void(const InputEvent&)> onInputEvent{ nullptr };
    };

    class Window
    {
    public:
        Window(std::string title, int width, int height, bool resizable);
        ~Window();

        void Close();
        bool ShouldClose();
        void Update();
        void WaitForFocus();

        void OnResize(std::function<void(int, int)> callback);
        void OnClose(std::function<void()> callback);

        void OnInputEvent(std::function<void(const InputEvent&)> func);

        std::unique_ptr<Vulkan::Surface> CreateSurface(Vulkan::Instance& instance);

        FramebufferSize GetFramebufferSize() const;
        void* GetHandle() const;

    private:
        GLFWwindow* handle;
        WindowCallbacks callbacks;

        friend class WindowBuilder;
    };

    class WindowBuilder
    {
    public:
        WindowBuilder Width(uint32_t width);
        WindowBuilder Height(uint32_t height);
        WindowBuilder Title(std::string title);
        std::unique_ptr<Window> Build();

    private:
        uint32_t width;
        uint32_t height;
        std::string title;
    };
}