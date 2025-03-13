#pragma once

#include "Image.h"
#include "Resource.h"

namespace Vulkan
{
	class Device;

	class ImageView : public Resource<VkImageView>
	{
	public:
		ImageView(const Device& device, Image& image, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D);
		~ImageView();

		const Image& GetImage() const;

		VkImageSubresourceRange GetSubresourceRange() const;

	private:
		const Device& device;
		Image& image;

		VkImageSubresourceRange subresourceRange;
	};
}