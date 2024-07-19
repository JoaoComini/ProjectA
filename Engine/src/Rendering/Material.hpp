#pragma once

#include <glm/glm.hpp>

#include "Texture.hpp"

#include "Resource/Resource.hpp"

#include "Vulkan/ShaderModule.hpp"

namespace Engine
{
	class Material : public Resource
	{
	public:
		Material(
			ResourceId albedoTexture,
			ResourceId normalTexture,
			ResourceId metallicRoughnessTexture,
			glm::vec4 albedoColor = glm::vec4{ 1.f },
			float metallicFactor = 0.f,
			float roughnessFactor = 0.f
		);

		~Material() = default;

		ResourceId GetAlbedoTexture() const;

		ResourceId GetNormalTexture() const;

		ResourceId GetMetallicRoughnessTexture() const;

		glm::vec4 GetAlbedoColor() const;

		float GetMetallicFactor() const;

		float GetRoughnessFactor() const;

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

		Vulkan::ShaderVariant shaderVariant;
	};
};