#include "Material.hpp"


namespace Engine
{
	Material::Material(Texture* diffuse) : diffuse(diffuse)
	{
	}

	Texture* Material::GetDiffuse() const
	{
		return this->diffuse;
	}

	glm::vec4 Material::GetColor() const
	{
		return this->color;
	}

};