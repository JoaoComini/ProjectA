#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Vulkan/Instance.hpp"

class Window
{
public:
    bool ShouldClose();
    void Update();
    void Destroy();
    void GetFrameBufferSize(int &width, int &height);
    void CreateSurface(Vulkan::Instance instance);

    VkSurfaceKHR GetSurface() const;

private:
    GLFWwindow *handle;
    VkSurfaceKHR surface;

    friend class WindowBuilder;
};

class WindowBuilder
{
public:
    WindowBuilder Width(uint32_t width);
    WindowBuilder Height(uint32_t height);
    Window Build();

private:
    uint32_t width;
    uint32_t heigth;
    Vulkan::Instance instance;
};