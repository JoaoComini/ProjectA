#pragma once

#include "Vulkan/Device.h"

#include "Rendering/Material.h"

#include "Resource/Resource.h"

#include "Factory.h"

namespace Engine
{
	struct MaterialSpec
	{
		ResourceId albedoTexture{ 0 };
		ResourceId normalTexture{ 0 };
		ResourceId metallicRoughnessTexture{ 0 };
		glm::vec4 albedoColor{ 1.f };
		float metallicFactor{ 0.f };
		float roughnessFactor{ 0.f };
		AlphaMode alphaMode{ AlphaMode::Opaque };
		float alphaCutoff{ 0.f };
	};

	class MaterialFactory : Factory<Material, MaterialSpec>
	{
	public:
		void Create(std::filesystem::path destination, MaterialSpec& spec) override;
		std::shared_ptr<Material> Load(std::filesystem::path source) override;
	};
};