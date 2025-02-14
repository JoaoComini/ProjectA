#pragma once

#include "Camera.hpp"

namespace Engine
{
    class RenderCamera : public Camera
    {
    public:

        void SetPosition(glm::vec3 position)
        {
            this->position = position;
        }

        glm::vec3 GetPosition() const
        {
            return position;
        }

        void SetRotation(glm::quat rotation)
        {
            this->rotation = rotation;
        }

        glm::quat GetRotation() const
        {
            return rotation;
        }

    private:
        glm::vec3 position{ 0.0f, 0.0f, 0.0f };
        glm::quat rotation{};
    };
}