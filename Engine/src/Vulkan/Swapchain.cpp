#include "Swapchain.h"

namespace Vulkan
{

	Swapchain::Swapchain(
		const Device& device,
		const Surface& surface,
		int width,
		int height) : device(device), surface(surface)
	{
		Setup(width, height);
	}

	void Swapchain::Setup(int width, int height, VkSwapchainKHR oldHandle)
	{
		Vulkan::SurfaceSupportDetails surfaceSupport = device.GetSurfaceSupportDetails();

		VkSurfaceFormatKHR surfaceFormat = Details::FindSurfaceFormat(surfaceSupport.formats);
		VkPresentModeKHR presentMode = Details::FindPresentMode(surfaceSupport.presentModes);
		VkExtent2D extent = Details::GetImageExtent(width, height, surfaceSupport.capabilities);

		uint32_t imageCount = surfaceSupport.capabilities.minImageCount + 1;

		if (surfaceSupport.capabilities.maxImageCount > 0 && imageCount > surfaceSupport.capabilities.maxImageCount)
		{
			imageCount = surfaceSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface.GetHandle();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t graphicsIndex = device.GetGraphicsQueueFamilyIndex();
		uint32_t presentIndex = device.GetPresentQueueFamilyIndex();

		uint32_t queueFamilyIndices[] = { graphicsIndex, presentIndex };

		if (graphicsIndex != presentIndex)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = surfaceSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = oldHandle;

		if (vkCreateSwapchainKHR(device.GetHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain!");
		}

		imageFormat = surfaceFormat.format;
		imageExtent = extent;

		CreateImageViews();

		if (oldHandle != nullptr)
		{
			vkDestroySwapchainKHR(device.GetHandle(), oldHandle, nullptr);
		}
	}

	void Swapchain::CreateImageViews()
	{
		CreateImages();

		imageViews.resize(images.size());

		for (size_t i = 0; i < images.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = images[i];

			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = imageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device.GetHandle(), &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create image views!");
			}
		}
	}

	void Swapchain::CreateImages()
	{
		uint32_t imageCount = 0;
		vkGetSwapchainImagesKHR(device.GetHandle(), handle, &imageCount, nullptr);
		images.resize(imageCount);
		vkGetSwapchainImagesKHR(device.GetHandle(), handle, &imageCount, images.data());
	}

	Swapchain::~Swapchain()
	{
		for (auto view : imageViews)
		{
			vkDestroyImageView(device.GetHandle(), view, nullptr);
		}

		vkDestroySwapchainKHR(device.GetHandle(), handle, nullptr);
	}

	void Swapchain::Recreate(int width, int height)
	{
		for (auto view : imageViews)
		{
			vkDestroyImageView(device.GetHandle(), view, nullptr);
		}

		images.clear();
		imageViews.clear();

		Setup(width, height, handle);
	}

	VkImageUsageFlags Swapchain::GetImageUsage() const
	{
		return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}

	VkFormat Swapchain::GetImageFormat() const
	{
		return imageFormat;
	}

	VkExtent2D Swapchain::GetImageExtent() const
	{
		return imageExtent;
	}

	std::vector<VkImage> Swapchain::GetImages() const
	{
		return images;
	}

	uint32_t Swapchain::GetImageCount() const
	{
		return images.size();
	}

	VkResult Swapchain::AcquireNextImageIndex(uint32_t& index, const Semaphore& acquireSemaphore)
	{
		return vkAcquireNextImageKHR(device.GetHandle(), handle, UINT64_MAX, acquireSemaphore.GetHandle(), VK_NULL_HANDLE, &index);
	}

	SwapchainBuilder SwapchainBuilder::DesiredWidth(int width)
	{
		desiredWidth = width;
		return *this;
	}

	SwapchainBuilder SwapchainBuilder::DesiredHeight(int height)
	{
		desiredHeight = height;
		return *this;
	}

	SwapchainBuilder SwapchainBuilder::MinImageCount(int minImageCount)
	{
		this->minImageCount = minImageCount;
		return *this;
	}

	std::unique_ptr<Swapchain> SwapchainBuilder::Build(const Device& device, const Surface& surface)
	{

		return std::make_unique<Swapchain>(
			device,
			surface,
			desiredWidth,
			desiredHeight);
	}

} // namespace Vulkan