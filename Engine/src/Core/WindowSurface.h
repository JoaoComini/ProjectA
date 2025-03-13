#pragma once

#include <GLFW/glfw3.h>

#include "Vulkan/Instance.h"
#include "Vulkan/Surface.h"


namespace Engine {

    class WindowSurface : public Vulkan::Surface
    {
    public:
        WindowSurface(Vulkan::Instance& instance, GLFWwindow* window);
    };

}