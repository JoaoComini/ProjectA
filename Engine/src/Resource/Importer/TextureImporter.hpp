#pragma once

#include "Vulkan/Device.hpp"

#include "Rendering/Texture.hpp"
#include "Rendering/Cubemap.hpp"

namespace Engine
{
	class TextureImporter
	{
	public:
		TextureImporter(Vulkan::Device &device);

		void ImportDefault(std::filesystem::path path);
		void ImportCubemap(std::filesystem::path path);

		std::shared_ptr<Texture> LoadDefault(std::filesystem::path path);
		std::shared_ptr<Texture> LoadDefault(const std::vector<uint8_t>& bytes);
		std::shared_ptr<Cubemap> LoadCubemap(std::filesystem::path path);

	private:
		Vulkan::Device& device;
	};
};