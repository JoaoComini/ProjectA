#include "ImageView.hpp"


namespace Vulkan
{

	ImageView::ImageView(const Device& device, Image& image, uint32_t mipLevels): device(device), image(image)
	{
        VkImageAspectFlags aspectMask = image.GetFormat() == VK_FORMAT_D32_SFLOAT ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;

        VkImageViewCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .image = image.GetHandle(),
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = image.GetFormat(),
            .subresourceRange = {
                .aspectMask = aspectMask,
                .baseMipLevel = 0,
                .levelCount = mipLevels,
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