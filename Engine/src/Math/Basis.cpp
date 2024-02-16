#include "Basis.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/orthonormalize.hpp>

#include <numbers>
#include <cmath>

#define EPSILON 0.00001f

namespace Engine
{
    Basis::Basis() { }

    Basis::Basis(glm::mat3 matrix)
    {
        this->matrix = matrix;
    }

    void Basis::SetQuaternion(glm::quat quaternion)
    {
        matrix = glm::mat4_cast(quaternion) * glm::mat4(1);
    }

    void Basis::SetEuler(glm::vec3 euler)
    {
        matrix = glm::eulerAngleXYZ(euler.x, euler.y, euler.z);
    }

    void Basis::SetEulerAndScale(glm::vec3 euler, glm::vec3 scale)
    {
        SetEuler(euler);

        glm::mat3 scaled{ 0 };
        scaled[0][0] = scale.x;
        scaled[1][1] = scale.y;
        scaled[2][2] = scale.z;

        matrix = matrix * scaled;
    }

    glm::vec3 Basis::GetEuler() const
    {
        glm::vec3 euler{ 0 };

        if (matrix[2][0] < 1 - EPSILON || matrix[2][0] > -(1 - EPSILON))
        {
            euler.x = atan2(-matrix[2][1], matrix[2][2]);
            euler.y = asin(matrix[2][0]);
            euler.z = atan2(-matrix[1][0], matrix[0][0]);
            return euler;
        }

        if (matrix[2][0] <= -(1 - EPSILON))
        {
            euler.x = atan2(matrix[1][2], matrix[1][1]);
            euler.y = -std::numbers::pi / 2.f;
            return euler;
        }

        euler.x = atan2(matrix[1][2], matrix[1][1]);
        euler.y = std::numbers::pi / 2.f;
        return euler;
    }

    glm::vec3 Basis::GetEulerNormalized() const
    {
        Basis result = Orthonormalized();

        float determinant = glm::determinant(result.matrix);

        if (determinant < 0)
        {
            result.matrix *= -1;
        }

        return result.GetEuler();
    }

    glm::vec3 Basis::GetScale() const
    {
        float sign = std::copysign(1.f, glm::determinant(matrix));

        auto x = glm::column(matrix, 0);
        auto y = glm::column(matrix, 1);
        auto z = glm::column(matrix, 2);

        return sign * glm::vec3(
            glm::length(x),
            glm::length(y),
            glm::length(z)
        );
    }

    Basis Basis::Orthonormalized() const
    {
        return Basis{ glm::orthonormalize(matrix) };
    }

    void Basis::Invert()
    {
        matrix = glm::inverse(matrix);
    }

    glm::vec3 Basis::XForm(const glm::vec3& vector) const
    {
        return glm::vec3
        {
            glm::dot(glm::row(matrix, 0), vector),
            glm::dot(glm::row(matrix, 1), vector),
            glm::dot(glm::row(matrix, 2), vector)
        };
    }

    glm::vec3 Basis::operator[](const int& row) const
    {
        return glm::row(matrix, row);
    }

    void Basis::operator*=(const Basis& other)
    {
        matrix *= other.matrix;
    }

    Basis Basis::operator*(const Basis& other) const
    {
        return Basis(matrix * other.matrix);
    }
}
