#pragma once

#include "Common/Singleton.hpp"

#include "KeyCode.hpp"
#include "Window.hpp"

namespace Engine
{
    class Input : public Singleton<Input>
    {
    public:
        static void Setup(Window& window);

        virtual bool IsKeyDown(KeyCode code) = 0;
    protected:
        Input() = default;
    };
};