#include "WindowInput.h"

#include "GLFW/glfw3.h"

namespace Engine
{
	int KeyCodeToGlfwKey(KeyCode code);

    WindowInput::WindowInput(Window& window)
        : window(window)
    {
    }

    bool WindowInput::IsKeyDown(KeyCode code)
    {
        return GLFW_PRESS == glfwGetKey(static_cast<GLFWwindow*>(window.GetHandle()), KeyCodeToGlfwKey(code));
    }

    bool WindowInput::IsMouseButtonDown(MouseButton button)
    {
        return GLFW_PRESS == glfwGetMouseButton(static_cast<GLFWwindow*>(window.GetHandle()), static_cast<int>(button));
    }

    glm::vec2 WindowInput::GetMousePosition()
    {
        double x, y;
        glfwGetCursorPos(static_cast<GLFWwindow*>(window.GetHandle()), &x, &y);

        return { (float)x, (float)y };
    }

    int KeyCodeToGlfwKey(KeyCode code)
    {
        switch (code)
        {
        case KeyCode::Tab:          return GLFW_KEY_TAB;
        case KeyCode::LeftShift:    return GLFW_KEY_LEFT_SHIFT;
        case KeyCode::LeftControl:  return GLFW_KEY_LEFT_CONTROL;
        case KeyCode::Space:        return GLFW_KEY_SPACE;
        case KeyCode::Enter:        return GLFW_KEY_ENTER;
        case KeyCode::Escape:       return GLFW_KEY_ESCAPE;
        case KeyCode::Backspace:    return GLFW_KEY_BACKSPACE;
        case KeyCode::Delete:       return GLFW_KEY_DELETE;

        case KeyCode::A: return GLFW_KEY_A;
        case KeyCode::B: return GLFW_KEY_B;
        case KeyCode::C: return GLFW_KEY_C;
        case KeyCode::D: return GLFW_KEY_D;
        case KeyCode::E: return GLFW_KEY_E;
        case KeyCode::F: return GLFW_KEY_F;
        case KeyCode::G: return GLFW_KEY_G;
        case KeyCode::H: return GLFW_KEY_H;
        case KeyCode::I: return GLFW_KEY_I;
        case KeyCode::J: return GLFW_KEY_J;
        case KeyCode::K: return GLFW_KEY_K;
        case KeyCode::L: return GLFW_KEY_L;
        case KeyCode::M: return GLFW_KEY_M;
        case KeyCode::N: return GLFW_KEY_N;
        case KeyCode::O: return GLFW_KEY_O;
        case KeyCode::P: return GLFW_KEY_P;
        case KeyCode::Q: return GLFW_KEY_Q;
        case KeyCode::R: return GLFW_KEY_R;
        case KeyCode::S: return GLFW_KEY_S;
        case KeyCode::T: return GLFW_KEY_T;
        case KeyCode::U: return GLFW_KEY_U;
        case KeyCode::V: return GLFW_KEY_V;
        case KeyCode::W: return GLFW_KEY_W;
        case KeyCode::X: return GLFW_KEY_X;
        case KeyCode::Y: return GLFW_KEY_Y;
        case KeyCode::Z: return GLFW_KEY_Z;

        case KeyCode::F1:  return GLFW_KEY_F1;
        case KeyCode::F2:  return GLFW_KEY_F2;
        case KeyCode::F3:  return GLFW_KEY_F3;
        case KeyCode::F4:  return GLFW_KEY_F4;
        case KeyCode::F5:  return GLFW_KEY_F5;
        case KeyCode::F6:  return GLFW_KEY_F6;
        case KeyCode::F7:  return GLFW_KEY_F7;
        case KeyCode::F8:  return GLFW_KEY_F8;
        case KeyCode::F9:  return GLFW_KEY_F9;
        case KeyCode::F10: return GLFW_KEY_F10;
        case KeyCode::F11: return GLFW_KEY_F11;
        case KeyCode::F12: return GLFW_KEY_F12;

        default: return GLFW_KEY_UNKNOWN;
        }
    }
}

