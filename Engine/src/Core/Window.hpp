#pragma once

#include <GLFW/glfw3.h>

#include <string>
#include <functional>

#include "Vulkan/Instance.hpp"
#include "Vulkan/Surface.hpp"


namespace Engine {

    struct FramebufferSize
    {
        int height;
        int width;
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
        void OnKey(std::function<void(int, int)> callback);
        void OnClose(std::function<void()> callback);

        std::unique_ptr<Vulkan::Surface> CreateSurface(Vulkan::Instance& instance);

        FramebufferSize GetFramebufferSize() const;
        GLFWwindow* GetHandle() const;

    private:
        GLFWwindow* handle;
        std::function<void(int, int)> onResizeFn = nullptr;
        std::function<void(int, int)> onKeyFn = nullptr;
        std::function<void()> onCloseFn = nullptr;


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