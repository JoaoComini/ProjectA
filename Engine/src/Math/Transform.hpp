#pragma once

#include "Basis.hpp"

#include <glm/glm.hpp>

namespace Engine
{
	struct Transform
	{
		Basis basis;
		glm::vec3 origin;

		glm::mat4 GetMatrix() const;

		void operator*=(const Transform& transform);
		Transform operator*(const Transform& transform) const;

		glm::vec3 XForm(const glm::vec3& vector) const;

		Transform AffineInverse() const;
	};
}