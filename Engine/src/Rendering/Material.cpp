#include "Material.hpp"


namespace Engine
{
	Material::Material(ResourceId diffuse, ResourceId normal, glm::vec4 color) : diffuse(diffuse), color(color)
	{ }

	ResourceId Material::GetDiffuse() const
	{
		return diffuse;
	}

	ResourceId Material::GetNormal() const
	{
		return normal;
	}

	glm::vec4 Material::GetColor() const
	{
		return color;
	}

};