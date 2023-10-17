#pragma once

#include "Common/Singleton.hpp"

#include "KeyCode.hpp"
#include "MouseCode.hpp"
#include "Window.hpp"

#include <glm/glm.hpp>

namespace Engine
{
    class Input : public Singleton<Input>
    {
    public:
        virtual bool IsKeyDown(KeyCode code) = 0;
        virtual bool IsMouseButtonDown(MouseCode code) = 0;
        virtual glm::vec2 GetMousePosition() = 0;
    protected:
        Input() = default;
    };
};