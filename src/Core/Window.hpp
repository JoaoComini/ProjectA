#pragma once

#include <GLFW/glfw3.h>

#include <string>

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

    FramebufferSize GetFramebufferSize() const;
    GLFWwindow *GetHandle() const;

private:
    GLFWwindow *handle;

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