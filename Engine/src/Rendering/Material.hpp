#pragma once

#include <glm/glm.hpp>

#include "Texture.hpp"

#include "Resource/Resource.hpp"

namespace Engine
{
	class Material : public Resource
	{
	public:
		Material(ResourceId diffuse);
		~Material() = default;

		ResourceId GetDiffuse() const;

		glm::vec4 GetColor() const;

	private:
		ResourceId diffuse;
		glm::vec4 color = glm::vec4(1.f, 1.f, 1.f, 1.1f);
	};
};