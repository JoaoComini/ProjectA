#include "Input.hpp"

#include "WindowInput.hpp"

namespace Engine
{
    void Input::Setup(Window& window)
    {
        if (instance == nullptr)
        {
            instance = new WindowInput(window);
        }
    }
};