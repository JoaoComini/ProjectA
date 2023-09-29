#pragma once

#include <GLFW/glfw3.h>

#include "Vulkan/Instance.hpp"
#include "Vulkan/Surface.hpp"


namespace Engine {

    class WindowSurface : public Vulkan::Surface
    {
    public:
        WindowSurface(Vulkan::Instance& instance, GLFWwindow* window);
    };

}