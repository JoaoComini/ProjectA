#pragma once

#include <vulkan/vulkan.h>

#include <limits>
#include <vector>

#include "Device.hpp"

namespace Vulkan
{

    class Swapchain
    {
    public:
        VkFormat GetImageFormat() const;
        VkExtent2D GetExtent() const;
        VkSwapchainKHR GetHandle() const;
        std::vector<VkImageView> GetImageViews() const;

    private:
        VkSwapchainKHR handle;
        VkFormat imageFormat;
        VkExtent2D extent;
        Device device;

        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;

        void CreateImages();
        void CreateImageViews();

        friend class SwapchainBuilder;
    };

    class SwapchainBuilder
    {

    public:
        SwapchainBuilder(Device device);
        SwapchainBuilder DesiredWidth(int width);
        SwapchainBuilder DesiredHeight(int height);

        Swapchain Build();

    private:
        VkSurfaceFormatKHR FindSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR FindPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D GetExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        Device device;
        int desiredWidth;
        int desiredHeight;
    };
} // namespace Vulkan
