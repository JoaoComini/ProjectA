#pragma once

#include <glm/glm.hpp>

#include "Texture.hpp"

namespace Rendering
{
	struct Material
	{
	public:
		Material(Texture* diffuse);
		~Material() = default;

		Texture* GetDiffuse() const;

		glm::vec4 GetColor() const;

	private:
		glm::vec4 color = glm::vec4(1.f, 1.f, 1.f, 1.1f);

		Texture* diffuse;
	};
};