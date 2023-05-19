#pragma once

#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"

namespace Rendering
{

	class Texture
	{
	public:
		Texture(const Vulkan::Device& device, std::string path);
		~Texture() = default;

		Vulkan::ImageView& GetImageView() const;

	private:
		const Vulkan::Device& device;
		std::unique_ptr<Vulkan::Image> image;
		std::unique_ptr<Vulkan::ImageView> imageView;
	};

}