#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Vulkan/Instance.hpp"
#include "Vulkan/Surface.hpp"

class WindowSurface : public Vulkan::Surface
{
public:
    WindowSurface(Vulkan::Instance &instance, GLFWwindow *window);

private:
    GLFWwindow *window;
};
