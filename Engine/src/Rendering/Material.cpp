#include "Material.hpp"


namespace Engine
{
	Material::Material(ResourceId diffuse, glm::vec4 color) : diffuse(diffuse), color(color)
	{ }

	ResourceId Material::GetDiffuse() const
	{
		return diffuse;
	}

	glm::vec4 Material::GetColor() const
	{
		return this->color;
	}

};