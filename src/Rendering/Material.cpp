#include "Material.hpp"


namespace Rendering
{
	Material::Material(Texture* diffuse) : diffuse(diffuse)
	{
	}

	Texture* Material::GetDiffuse() const
	{
		return this->diffuse;
	}

};