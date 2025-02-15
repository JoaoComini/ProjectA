#pragma once

#include "Common/Singleton.hpp"

#include "InputEvent.hpp"
#include "Window.hpp"

namespace Engine
{
    class Input : public Singleton<Input>
    {
    public:
        virtual bool IsKeyDown(KeyCode code) = 0;
        virtual bool IsMouseButtonDown(MouseButton button) = 0;
        virtual glm::vec2 GetMousePosition() = 0;
    protected:
        Input() = default;
    };
};