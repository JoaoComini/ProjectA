#pragma once

#include "Input.hpp"
#include "Window.hpp"

#include "KeyCode.hpp"

#include <unordered_map>

namespace Engine
{
    class WindowInput : public Input
    {
    public:
        WindowInput(Window& window);

        bool IsKeyDown(KeyCode code) override;

    private:
        std::unordered_map<int, bool> keys;
    };
};