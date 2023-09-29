#include "WindowInput.hpp"

#include "GLFW/glfw3.h"

namespace Engine
{
    WindowInput::WindowInput(Window& window)
    {
        window.OnKey([&](int key, int action) {
            keys[key] = action != GLFW_RELEASE;
        });
    }

    bool WindowInput::IsKeyDown(KeyCode key)
    {
        return keys[key];
    }
}

