#pragma once

namespace Engine
{
    class Entity;

    class TransformHelper
    {

    public:
        static glm::mat4 ComputeEntityWorldMatrix(Entity entity);

        static bool MatrixDecompose(const glm::mat4& matrix, glm::vec3& position, glm::vec3& rotation);

    private:
        TransformHelper() = default;
    };

}