#pragma once

#include "Input.hpp"
#include "Window.hpp"

#include "KeyCode.hpp"

namespace Engine
{
    class WindowInput : public Input
    {
    public:
        WindowInput(Window& window);

        bool IsKeyDown(KeyCode code) override;
        bool IsMouseButtonDown(MouseCode code) override;

        glm::vec2 GetMousePosition() override;

    private:
        std::unordered_map<int, bool> keys;

        Window& window;
    };
};