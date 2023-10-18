#include "Material.hpp"


namespace Engine
{
	Material::Material(ResourceId diffuse) : diffuse(diffuse)
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