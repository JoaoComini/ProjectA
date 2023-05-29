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

	private:
		glm::vec4 color = glm::vec4(1.0f, 0.f, 0.f, 1.0f);

		Texture* diffuse;
	};
};