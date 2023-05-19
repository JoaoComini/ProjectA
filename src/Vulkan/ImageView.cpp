#include "ImageView.hpp"


namespace Vulkan
{

	ImageView::ImageView(const Device& device, Image& image, ImageFormat format): device(device), image(image)
	{
        VkImageAspectFlags aspectMask = format == ImageFormat::D32_SFLOAT ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;

        VkImageViewCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .image = image.GetHandle(),
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = static_cast<VkFormat>(format),
            .subresourceRange = {
                .aspectMask = aspectMask,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        vkCreateImageView(device.GetHandle(), &createInfo, nullptr, &handle);
	}

	ImageView::~ImageView()
	{
        vkDestroyImageView(device.GetHandle(), handle, nullptr);
	}
}