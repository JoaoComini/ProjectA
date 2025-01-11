#include "Material.hpp"

namespace Engine
{
	Material::Material(
		ResourceId albedoTexture,
		ResourceId normalTexture,
		ResourceId metallicRoughnessTexture,
		glm::vec4 albedoColor,
		float metallicFactor,
		float roughnessFactor,
		AlphaMode alphaMode,
		float alphaCutoff
	) : albedoTexture(albedoTexture), normalTexture(normalTexture), metallicRoughnessTexture(metallicRoughnessTexture),
		albedoColor(albedoColor), metallicFactor(metallicFactor), roughnessFactor(roughnessFactor), alphaMode(alphaMode), alphaCutoff(alphaCutoff)
	{
		PrepareShaderVariant();
	}

	void Material::PrepareShaderVariant()
	{
		if (albedoTexture)
		{
			shaderVariant.AddDefine("HAS_ALBEDO_TEXTURE");
		}

		if (normalTexture)
		{
			shaderVariant.AddDefine("HAS_NORMAL_TEXTURE");
		}

		if (metallicRoughnessTexture)
		{
			shaderVariant.AddDefine("HAS_METALLIC_ROUGHNESS_TEXTURE");
		}

		if (alphaMode == AlphaMode::Mask)
		{
			shaderVariant.AddDefine("ALPHA_MASK");
		}
	}

	ResourceId Material::GetAlbedoTexture() const
	{
		return albedoTexture;
	}

	ResourceId Material::GetNormalTexture() const
	{
		return normalTexture;
	}

	ResourceId Material::GetMetallicRoughnessTexture() const
	{
		return metallicRoughnessTexture;
	}

	glm::vec4 Material::GetAlbedoColor() const
	{
		return albedoColor;
	}

	float Material::GetMetallicFactor() const
	{
		return metallicFactor;
	}

	float Material::GetRoughnessFactor() const
	{
		return roughnessFactor;
	}

	AlphaMode Material::GetAlphaMode() const
	{
		return alphaMode;
	}

	float Material::GetAlphaCutoff() const
	{
		return alphaCutoff;
	}

	const Vulkan::ShaderVariant& Material::GetShaderVariant() const
	{
		return shaderVariant;
	}

};