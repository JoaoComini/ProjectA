#pragma once

#include "Texture.h"

#include "Common/Hash.h"
#include "Resource/Resource.h"
#include "Shader.h"

namespace glm
{
	template <class Archive>
	void Serialize(Archive& ar, vec4& vec)
	{
		ar(vec.x, vec.y, vec.z, vec.w);
	}
}

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
		Material() = default;

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

		[[nodiscard]] ResourceType GetType() const override
		{
			return ResourceType::Material;
		}

		[[nodiscard]] const ShaderVariant& GetShaderVariant() const;

		template<typename Archive>
		void Serialize(Archive& ar)
		{
			ar(albedoTexture, normalTexture, metallicRoughnessTexture);
			ar(albedoColor, metallicFactor, roughnessFactor);
			ar(alphaMode, alphaCutoff);

			PrepareShaderVariant();
		}

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

		HashCombine(hash, material.GetAlbedoTexture()->GetId());
		HashCombine(hash, material.GetNormalTexture()->GetId());
		HashCombine(hash, material.GetMetallicRoughnessTexture()->GetId());
		HashCombine(hash, material.GetAlbedoColor());
		HashCombine(hash, material.GetMetallicFactor());
		HashCombine(hash, material.GetRoughnessFactor());
		HashCombine(hash, material.GetAlphaMode());
		HashCombine(hash, material.GetAlphaCutoff());
		HashCombine(hash, material.GetShaderVariant().GetHash());

		return hash;
	}
};;