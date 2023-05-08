#pragma once

#include <vulkan/vulkan.h>

#include <limits>
#include <vector>

#include "Device.hpp"
#include "Surface.hpp"
#include "Details.hpp"
#include "Semaphore.hpp"

namespace Vulkan
{

    class Swapchain
    {
    public:
        Swapchain(
            const Device &device,
            const Surface &surface,
            int width,
            int height,
            int maxFramesInFlight);

        ~Swapchain();

        void Recreate(int width, int height);

        VkFormat GetImageFormat() const;
        VkExtent2D GetImageExtent() const;
        VkSwapchainKHR GetHandle() const;
        std::vector<VkImageView> GetImageViews() const;
        uint32_t GetNextImageIndex(uint32_t currentFrame);
        Semaphore& GetSemaphore(uint32_t currentFrame);

    private:
        VkSwapchainKHR handle;
        VkFormat imageFormat;
        VkExtent2D imageExtent;
        std::vector<Semaphore> semaphores;

        const Device &device;
        const Surface &surface;

        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;

        int maxFramesInFlight;

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
        SwapchainBuilder MaxFramesInFlight(int maxFramesInFlight);
        std::unique_ptr<Swapchain> Build(const Device &device, const Surface &surface);

    private:
        int desiredWidth;
        int desiredHeight;
        int maxFramesInFlight = 1;
    };
} // namespace Vulkan
