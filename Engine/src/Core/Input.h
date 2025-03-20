#pragma once

#include "Common/Singleton.h"

#include "InputEvent.h"
#include "Window.h"

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