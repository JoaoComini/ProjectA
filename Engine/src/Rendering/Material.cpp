#include "Material.hpp"

namespace Engine
{
	Material::Material(ResourceId diffuse, ResourceId normal, glm::vec4 color)
		: diffuse(diffuse), normal(normal), color(color)
	{
		PrepareShaderVariant();
	}

	void Material::PrepareShaderVariant()
	{
		if (diffuse)
		{
			shaderVariant.AddDefine("HAS_DIFFUSE_TEXTURE");
		}

		if (normal)
		{
			shaderVariant.AddDefine("HAS_NORMAL_TEXTURE");
		}
	}

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

	const Vulkan::ShaderVariant& Material::GetShaderVariant() const
	{
		return shaderVariant;
	}

};