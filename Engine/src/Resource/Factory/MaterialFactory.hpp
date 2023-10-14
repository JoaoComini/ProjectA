#pragma once

#include "Vulkan/Device.hpp"

#include "Rendering/Material.hpp"

#include "Resource/Resource.hpp"

namespace Engine
{
	struct MaterialSpec
	{
		ResourceId diffuse{0};
	};

	class MaterialFactory
	{
	public:
		ResourceId Create(std::filesystem::path destination, MaterialSpec& spec);
		std::shared_ptr<Material> Load(std::filesystem::path source);
	};
};