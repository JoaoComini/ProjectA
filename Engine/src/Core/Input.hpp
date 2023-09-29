#pragma once

#include "KeyCode.hpp"
#include "Window.hpp"

namespace Engine
{
    class Input
    {
    public:
        virtual ~Input();
        virtual bool IsKeyDown(KeyCode code) = 0;

        static void Setup(Window& window);
        static Input* GetInstance();

    protected:
        static Input* instance;
    };
};