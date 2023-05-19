#pragma once

#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"

namespace Rendering
{

	class Texture
	{
	public:
		Texture(const Vulkan::Device& device, std::string path);
		~Texture() = default;

		Vulkan::Image& GetImage() const;

	private:
		const Vulkan::Device& device;
		std::unique_ptr<Vulkan::Image> image;
	};

}