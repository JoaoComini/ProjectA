#pragma once

#include "Device.h"
#include "Surface.h"
#include "Details.h"
#include "Semaphore.h"
#include "Resource.h"

namespace Vulkan
{

	class Swapchain : public Resource<VkSwapchainKHR>
	{
	public:
		Swapchain(
			const Device& device,
			const Surface& surface,
			int width,
			int height);

		~Swapchain();

		VkResult AcquireNextImageIndex(uint32_t& index, const Semaphore& acquireSemaphore);

		void Recreate(int width, int height);

		VkImageUsageFlags GetImageUsage() const;
		VkFormat GetImageFormat() const;
		VkExtent2D GetImageExtent() const;
		std::vector<VkImage> GetImages() const;
		uint32_t GetImageCount() const;

	private:
		VkFormat imageFormat;
		VkExtent2D imageExtent;

		const Device& device;
		const Surface& surface;

		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;

		void Setup(int width, int height, VkSwapchainKHR old = VK_NULL_HANDLE);

		void CreateImages();
		void CreateImageViews();

		friend class SwapchainBuilder;
	};

	class SwapchainBuilder
	{

	public:
		SwapchainBuilder() = default;
		SwapchainBuilder DesiredWidth(int width);
		SwapchainBuilder DesiredHeight(int height);
		SwapchainBuilder MinImageCount(int minImageCount);
		std::unique_ptr<Swapchain> Build(const Device& device, const Surface& surface);

	private:
		int desiredWidth{ 0 };
		int desiredHeight{ 0 };
		int minImageCount{ 2 };
	};
} // namespace Vulkan
