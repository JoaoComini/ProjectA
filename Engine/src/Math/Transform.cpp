#include "Transform.hpp"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <numbers>

namespace Engine
{
    glm::mat4 Transform::GetMatrix() const
    {
        glm::mat4 matrix{ basis.matrix };

        matrix[3] = glm::vec4{ origin, 1 };

        return matrix;
    }

    void Transform::operator*=(const Transform& transform)
    {
        origin = XForm(transform.origin);
        basis *= transform.basis;
    }

    Transform Transform::operator*(const Transform& transform) const
    {
        Transform result{*this};

        result *= transform;

        return result;
    }


    glm::vec3 Transform::XForm(const glm::vec3& vector) const
    {
        return basis.XForm(vector) + origin;
    }

    Transform Transform::AffineInverse() const
    {
        Transform result = *this;
        result.basis.Invert();
        result.origin = result.basis.XForm(-result.origin);

        return result;
    }
}
