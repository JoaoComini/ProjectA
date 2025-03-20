#include "Material.h"

namespace Engine
{
	Material::Material(
		std::shared_ptr<Texture> albedoTexture,
		std::shared_ptr<Texture> normalTexture,
		std::shared_ptr<Texture> metallicRoughnessTexture,
		const glm::vec4 albedoColor,
		const float metallicFactor,
		const float roughnessFactor,
		const AlphaMode alphaMode,
		const float alphaCutoff
	) : albedoTexture(albedoTexture), normalTexture(normalTexture), metallicRoughnessTexture(metallicRoughnessTexture),
		albedoColor(albedoColor), metallicFactor(metallicFactor), roughnessFactor(roughnessFactor), alphaMode(alphaMode), alphaCutoff(alphaCutoff)
	{
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

	Texture* Material::GetAlbedoTexture() const
	{
		return albedoTexture.get();
	}

	Texture* Material::GetNormalTexture() const
	{
		return normalTexture.get();
	}

	Texture* Material::GetMetallicRoughnessTexture() const
	{
		return metallicRoughnessTexture.get();
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

	const ShaderVariant& Material::GetShaderVariant() const
	{
		return shaderVariant;
	}

};