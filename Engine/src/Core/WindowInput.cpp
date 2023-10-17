#include "WindowInput.hpp"

#include "GLFW/glfw3.h"

namespace Engine
{
    WindowInput::WindowInput(Window& window)
        : window(window)
    {
        window.OnKey([&](int key, int action) {
            keys[key] = action != GLFW_RELEASE;
        });
    }

    bool WindowInput::IsKeyDown(KeyCode key)
    {
        return keys[key];
    }
    bool WindowInput::IsMouseButtonDown(MouseCode code)
    {
        return GLFW_PRESS == glfwGetMouseButton(static_cast<GLFWwindow*>(window.GetHandle()), code);
    }
    glm::vec2 WindowInput::GetMousePosition()
    {
        double x, y;
        glfwGetCursorPos(static_cast<GLFWwindow*>(window.GetHandle()), &x, &y);

        return { (float)x, (float)y };
    }
}

