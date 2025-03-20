#pragma once

#include "Vulkan/Device.h"

#include "Rendering/Texture.h"
#include "Rendering/Cubemap.h"

namespace Engine
{
	class TextureImporter
	{
	public:
		void ImportDefault(std::filesystem::path path);
		void ImportCubemap(std::filesystem::path path);

		std::shared_ptr<Texture> LoadDefault(std::filesystem::path path);
		std::shared_ptr<Texture> LoadDefault(const std::vector<uint8_t>& bytes);
		std::shared_ptr<Cubemap> LoadCubemap(std::filesystem::path path);
	};
};