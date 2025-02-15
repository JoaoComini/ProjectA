#pragma once

#include "Input.hpp"
#include "Window.hpp"

#include "InputEvent.hpp"

namespace Engine
{
    class WindowInput : public Input
    {
    public:
        WindowInput(Window& window);

        bool IsKeyDown(KeyCode code) override;
        bool IsMouseButtonDown(MouseButton code) override;

        glm::vec2 GetMousePosition() override;

    private:
        std::unordered_map<KeyCode, bool> keys;

        Window& window;
    };
};