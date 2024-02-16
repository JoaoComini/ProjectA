#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine
{
	struct Basis
	{
	public:
		Basis();
		Basis(glm::mat3);

		void SetQuaternion(glm::quat quaternion);

		void SetEuler(glm::vec3 euler);
		glm::vec3 GetEuler() const;
		glm::vec3 GetEulerNormalized() const;

		void SetEulerAndScale(glm::vec3 euler, glm::vec3 scale);
		glm::vec3 GetScale() const;

		Basis Orthonormalized() const;

		void Invert();

		glm::vec3 XForm(const glm::vec3& vector) const;

		glm::vec3 operator[](const int& row) const;

		void operator*=(const Basis& other);
		Basis operator*(const Basis& other) const;

		glm::mat3 matrix{ 1 };
	};

}