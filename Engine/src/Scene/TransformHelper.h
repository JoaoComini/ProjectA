#pragma once

#include "SceneGraph.h"

namespace Engine
{
    class TransformHelper
    {

    public:
        static glm::mat4 ComputeEntityWorldMatrix(SceneGraph& scene, Entity::Id entity);

        static bool MatrixDecompose(const glm::mat4& matrix, glm::vec3& position, glm::vec3& rotation);

    private:
        TransformHelper() = default;
    };

}