#pragma once

#include "Vulkan/Device.hpp"

#include "Rendering/Texture.hpp"

#include <filesystem>

namespace Engine
{
	class TextureImporter
	{
	public:
		TextureImporter(Vulkan::Device &device);

		std::shared_ptr<Texture> Import(std::filesystem::path path);
	private:
		const Vulkan::Device& device;
	};
};