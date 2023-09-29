#include "Input.hpp"

#include "WindowInput.hpp"

namespace Engine
{
    Input* Input::instance = nullptr;;

    Input::~Input()
    {
        if (instance != nullptr)
        {
            delete instance;
        }
    }

    void Input::Setup(Window& window)
    {
        if (instance == nullptr)
        {
            instance = new WindowInput(window);
        }
    }

    Input* Input::GetInstance()
    {
        return instance;
    }
};