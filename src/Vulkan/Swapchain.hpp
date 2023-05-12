#pragma once

#include <vulkan/vulkan.h>

#include <limits>
#include <vector>

#include "Device.hpp"
#include "Surface.hpp"
#include "Details.hpp"
#include "Semaphore.hpp"
#include "Resource.hpp"

namespace Vulkan
{

    class Swapchain: public Resource<VkSwapchainKHR>
    {
    public:
        Swapchain(
            const Device &device,
            const Surface &surface,
            int width,
            int height);

        ~Swapchain();

        uint32_t AcquireNextImageIndex(const Semaphore& acquireSemaphore);

        void Recreate(int width, int height);

        VkFormat GetImageFormat() const;
        VkExtent2D GetImageExtent() const;
        std::vector<VkImageView> GetImageViews() const;
        uint32_t GetImageCount() const;


    private:
        VkFormat imageFormat;
        VkExtent2D imageExtent;
        std::vector<Semaphore> semaphores;

        const Device &device;
        const Surface &surface;

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
        std::unique_ptr<Swapchain> Build(const Device &device, const Surface &surface);

    private:
        int desiredWidth;
        int desiredHeight;
        int minImageCount = 2;
    };
} // namespace Vulkan
