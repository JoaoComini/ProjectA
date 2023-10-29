#pragma once

#include <vulkan/vulkan.h>

#include "Image.hpp"
#include "Resource.hpp"

namespace Vulkan
{
	class Device;

	class ImageView : public Resource<VkImageView>
	{
	public:
		ImageView(const Device& device, Image& image, uint32_t mipLevels = 1);
		~ImageView();

		const Image& GetImage() const;

		VkImageSubresourceRange GetSubresourceRange() const;

	private:
		const Device& device;
		Image& image;

		VkImageSubresourceRange subresourceRange;
	};
}