#pragma once

#define GLFW_INCLUDE_VULKAN
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
    Window(std::string name, int width, int height, void *userPointer, bool resizable);
    ~Window();

    bool ShouldClose();
    void Update();
    void WaitForFocus();

    FramebufferSize GetFramebufferSize();
    GLFWwindow *GetHandle() const;

    template <class T>
    T *GetUserPointer() { return static_cast<T *>(userPointer); }

    void SetFramebufferSizeCallback(void (*callback)(Window *window, FramebufferSize size));

private:
    GLFWwindow *handle;
    void *userPointer = nullptr;

    void (*framebufferSizeCallback)(Window *window, FramebufferSize size);

    friend class WindowBuilder;
};

class WindowBuilder
{
public:
    WindowBuilder Width(uint32_t width);
    WindowBuilder Height(uint32_t height);
    WindowBuilder UserPointer(void *userPointer);
    std::unique_ptr<Window> Build();

private:
    uint32_t width;
    uint32_t height;
    void *userPointer = nullptr;
};