#pragma once

#include "Texture.h"

#include "Common/Hash.h"
#include "Resource/Resource.h"
#include "Shader.h"

namespace Engine
{
	enum class AlphaMode
	{
		Opaque,
		Mask,
		Blend
	};

	class Material final : public Resource
	{
	public:
		Material(
			std::shared_ptr<Texture> albedoTexture,
			std::shared_ptr<Texture> normalTexture,
			std::shared_ptr<Texture> metallicRoughnessTexture,
			glm::vec4 albedoColor = glm::vec4{ 1.f },
			float metallicFactor = 0.f,
			float roughnessFactor = 0.f,
			AlphaMode alphaMode = AlphaMode::Opaque,
			float alphaCutoff = 0.5f
		);

		~Material() override = default;

		[[nodiscard]] Texture* GetAlbedoTexture() const;

		[[nodiscard]] Texture* GetNormalTexture() const;

		[[nodiscard]] Texture* GetMetallicRoughnessTexture() const;

		[[nodiscard]] glm::vec4 GetAlbedoColor() const;

		[[nodiscard]] float GetMetallicFactor() const;

		[[nodiscard]] float GetRoughnessFactor() const;

		[[nodiscard]] AlphaMode GetAlphaMode() const;

		[[nodiscard]] float GetAlphaCutoff() const;

		static ResourceType GetStaticType()
		{
			return ResourceType::Material;
		}

		[[nodiscard]] ResourceType GetType() const override
		{
			return GetStaticType();
		}

		static std::string GetExtension()
		{
			return "pares";
		}

		[[nodiscard]] const ShaderVariant& GetShaderVariant() const;

	private:
		void PrepareShaderVariant();

		std::shared_ptr<Texture> albedoTexture;
		std::shared_ptr<Texture> normalTexture;
		std::shared_ptr<Texture> metallicRoughnessTexture;

		glm::vec4 albedoColor{ 1.f };
		float metallicFactor{ 0.f };
		float roughnessFactor{ 0.f };

		AlphaMode alphaMode{ AlphaMode::Opaque };
		float alphaCutoff{ 0.5f };

		ShaderVariant shaderVariant;
	};
};

template <>
struct std::hash<Engine::Material>
{
	size_t operator()(const Engine::Material& material) const noexcept
	{
		size_t hash{ 0 };

		HashCombine(hash, material.GetAlbedoTexture()->id);
		HashCombine(hash, material.GetNormalTexture()->id);
		HashCombine(hash, material.GetMetallicRoughnessTexture()->id);
		HashCombine(hash, material.GetAlbedoColor());
		HashCombine(hash, material.GetMetallicFactor());
		HashCombine(hash, material.GetRoughnessFactor());
		HashCombine(hash, material.GetAlphaMode());
		HashCombine(hash, material.GetAlphaCutoff());

		return hash;
	}
};;