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

    class Window
    {
    public:
        Window(std::string title, int width, int height, bool resizable);
        ~Window();

        void Close();
        bool ShouldClose();
        void Update();
        void WaitForFocus();

        void SetCursor(WindowCursor cursor);

        void OnResize(std::function<void(int, int)> callback);
        void OnClose(std::function<void()> callback);

        void OnInputEvent(std::function<void(const InputEvent&)> func);

        std::unique_ptr<Vulkan::Surface> CreateSurface(Vulkan::Instance& instance);

        FramebufferSize GetFramebufferSize() const;
        void* GetHandle() const;

    private:
        GLFWwindow* handle;
        std::function<void(int, int)> onResizeFn = nullptr;
        std::function<void()> onCloseFn = nullptr;
        std::function<void(const InputEvent&)> onInputEventFn{ nullptr };

        std::unordered_map<WindowCursor, GLFWcursor*> cursors;


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