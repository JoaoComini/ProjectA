#include "ImageView.hpp"

#include "Device.hpp"

namespace Vulkan
{

	ImageView::ImageView(const Device& device, Image& image, VkImageViewType viewType): device(device), image(image)
	{
        VkImageAspectFlags aspectMask = image.GetFormat() == VK_FORMAT_D32_SFLOAT ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;

        subresourceRange = {
                .aspectMask = aspectMask,
                .baseMipLevel = 0,
                .levelCount = image.GetMipLevels(),
                .baseArrayLayer = 0,
                .layerCount = image.GetArrayLayers(),
        };

        VkImageViewCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .image = image.GetHandle(),
            .viewType = viewType,
            .format = image.GetFormat(),
            .subresourceRange = subresourceRange,
        };

        vkCreateImageView(device.GetHandle(), &createInfo, nullptr, &handle);
	}

    const Image& ImageView::GetImage() const
    {
        return image;
    }

    VkImageSubresourceRange ImageView::GetSubresourceRange() const
    {
        return subresourceRange;
    }

	ImageView::~ImageView()
	{
        vkDestroyImageView(device.GetHandle(), handle, nullptr);
	}
}