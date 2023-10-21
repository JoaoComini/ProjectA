#pragma once

#include "Vulkan/Device.hpp"

#include "Rendering/Material.hpp"

#include "Resource/Resource.hpp"

#include "Factory.hpp"

namespace Engine
{
	struct MaterialSpec
	{
		ResourceId diffuse{0};
		glm::vec4 color{1.f};
	};

	class MaterialFactory : Factory<Material, MaterialSpec>
	{
	public:
		void Create(std::filesystem::path destination, MaterialSpec& spec) override;
		std::shared_ptr<Material> Load(std::filesystem::path source) override;
	};
};