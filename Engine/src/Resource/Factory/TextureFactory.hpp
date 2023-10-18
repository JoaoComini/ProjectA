#pragma once

#include "Vulkan/Device.hpp"
#include "Rendering/Texture.hpp"

#include "Factory.hpp"

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

	class TextureFactory : Factory<Texture, TextureSpec>
	{
	public:
		TextureFactory(const Vulkan::Device& device);

		void Create(std::filesystem::path destination, TextureSpec& spec) override;
		std::shared_ptr<Texture> Load(std::filesystem::path source) override;

	private:
		const Vulkan::Device& device;
	};
};
