#pragma once

#include <vulkan/vulkan.h>

#include "Device.hpp"
#include "Image.hpp"
#include "Resource.hpp"

namespace Vulkan
{

	class ImageView : public Resource<VkImageView>
	{
	public:
		ImageView(const Device& device, Image& image, ImageFormat format);
		~ImageView();

	private:
		const Device& device;
		Image& image;

	};
}