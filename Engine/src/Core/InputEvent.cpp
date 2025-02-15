#include "InputEvent.hpp"
#pragma once

namespace Engine
{

    KeyInputEvent::KeyInputEvent(KeyCode code, KeyAction action)
        :code(code), action(action)
    {
    }

    KeyCode KeyInputEvent::GetCode() const
    {
        return code;
    }

    KeyAction KeyInputEvent::GetAction() const
    {
        return action;
    }

    std::type_index KeyInputEvent::GetType() const
    {
        return typeid(KeyInputEvent);
    }

    MouseButtonInputEvent::MouseButtonInputEvent(MouseButton button, MouseButtonAction action)
        : button(button), action(action)
    {
    }

    MouseButton MouseButtonInputEvent::GetButton() const
    {
        return button;
    }

    MouseButtonAction MouseButtonInputEvent::GetAction() const
    {
        return action;
    }

    std::type_index MouseButtonInputEvent::GetType() const
    {
        return typeid(MouseButtonInputEvent);
    }

    MouseMoveInputEvent::MouseMoveInputEvent(float x, float y)
        : x(x), y(y)
    {
    }

    float MouseMoveInputEvent::GetX() const
    {
        return x;
    }

    float MouseMoveInputEvent::GetY() const
    {
        return y;
    }

    std::type_index MouseMoveInputEvent::GetType() const
    {
        return typeid(MouseMoveInputEvent);
    }
}
