#pragma once

#include "Camera.h"

namespace Engine
{
    class RenderCamera : public Camera
    {
    public:

        void SetPosition(glm::vec3 position)
        {
            this->position = position;
        }

        [[nodiscard]] glm::vec3 GetPosition() const
        {
            return position;
        }

        void SetRotation(glm::quat rotation)
        {
            this->rotation = rotation;
        }

        [[nodiscard]] glm::quat GetRotation() const
        {
            return rotation;
        }

        [[nodiscard]] glm::mat4 GetTransform() const
        {
            return translate(glm::mat4(1.0f), position)
                * mat4_cast(rotation);
        }


    private:
        glm::vec3 position{ 0.0f, 0.0f, 0.0f };
        glm::quat rotation{};
    };
}