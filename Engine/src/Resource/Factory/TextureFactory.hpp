#pragma once

#include "Vulkan/Device.hpp"
#include "Rendering/Texture.hpp"

#include "Factory.hpp"

namespace Engine
{
	class TextureFactory : Factory<Texture, Texture>
	{
	public:
		TextureFactory(Vulkan::Device& device);

		void Create(std::filesystem::path destination, Texture& texture) override;
		std::shared_ptr<Texture> Load(std::filesystem::path source) override;

	private:
		Vulkan::Device& device;
	};
};
