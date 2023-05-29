#pragma once

#include <GLFW/glfw3.h>

#include <string>
#include <functional>

#include "Vulkan/Instance.hpp"

struct FramebufferSize
{
    int height;
    int width;
};

class Window
{
public:
    Window(std::string name, int width, int height, bool resizable);
    ~Window();

    bool ShouldClose();
    void Update();
    void WaitForFocus();

    void OnResize(std::function<void(int, int)> callback);

    FramebufferSize GetFramebufferSize() const;
    GLFWwindow *GetHandle() const;

private:
    GLFWwindow *handle;
    std::function<void(int, int)> resizeFn = nullptr;


    friend class WindowBuilder;
};

class WindowBuilder
{
public:
    WindowBuilder Width(uint32_t width);
    WindowBuilder Height(uint32_t height);
    std::unique_ptr<Window> Build();

private:
    uint32_t width;
    uint32_t height;
};