#pragma once

#include "Texture.hpp"

#include "Common/Hash.hpp"
#include "Resource/Resource.hpp"
#include "Vulkan/ShaderModule.hpp"

namespace Engine
{
	enum class AlphaMode
	{
		Opaque,
		Mask,
		Blend
	};

	class Material : public Resource
	{
	public:
		Material(
			ResourceId albedoTexture,
			ResourceId normalTexture,
			ResourceId metallicRoughnessTexture,
			glm::vec4 albedoColor = glm::vec4{ 1.f },
			float metallicFactor = 0.f,
			float roughnessFactor = 0.f,
			AlphaMode alphaMode = AlphaMode::Opaque,
			float alphaCutoff = 0.5f
		);

		~Material() = default;

		ResourceId GetAlbedoTexture() const;

		ResourceId GetNormalTexture() const;

		ResourceId GetMetallicRoughnessTexture() const;

		glm::vec4 GetAlbedoColor() const;

		float GetMetallicFactor() const;

		float GetRoughnessFactor() const;

		AlphaMode GetAlphaMode() const;

		float GetAlphaCutoff() const;

		static ResourceType GetStaticType()
		{
			return ResourceType::Material;
		}

		virtual ResourceType GetType() const override
		{
			return GetStaticType();
		}

		static std::string GetExtension()
		{
			return "pares";
		}

		const Vulkan::ShaderVariant& GetShaderVariant() const;

	private:
		void PrepareShaderVariant();

		ResourceId albedoTexture;
		ResourceId normalTexture;
		ResourceId metallicRoughnessTexture;

		glm::vec4 albedoColor{ 1.f };
		float metallicFactor{ 0.f };
		float roughnessFactor{ 0.f };

		AlphaMode alphaMode{ AlphaMode::Opaque };
		float alphaCutoff{ 0.5f };

		Vulkan::ShaderVariant shaderVariant;
	};
};

namespace std
{
	template <>
	struct hash<Engine::Material>
	{
		size_t operator()(const Engine::Material& material) const
		{
			size_t hash{ 0 };

			HashCombine(hash, material.GetAlbedoTexture());
			HashCombine(hash, material.GetNormalTexture());
			HashCombine(hash, material.GetMetallicRoughnessTexture());
			HashCombine(hash, material.GetAlbedoColor());
			HashCombine(hash, material.GetMetallicFactor());
			HashCombine(hash, material.GetRoughnessFactor());
			HashCombine(hash, material.GetAlphaMode());
			HashCombine(hash, material.GetAlphaCutoff());

			return hash;
		}
	};
};