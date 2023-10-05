#pragma once

#include "Vulkan/Device.hpp"
#include "Rendering/Texture.hpp"

#include <vector>
#include <filesystem>

namespace Engine
{

	struct TextureSpec
	{
		int width;
		int height;
		int component;
		std::vector<uint8_t> image;
	};

	class TextureFactory
	{
	public:
		TextureFactory(const Vulkan::Device& device);

		std::shared_ptr<Texture> Create(std::filesystem::path destination, TextureSpec& spec);
		std::shared_ptr<Texture> Load(std::filesystem::path source);

	private:
		const Vulkan::Device& device;
	};
};
