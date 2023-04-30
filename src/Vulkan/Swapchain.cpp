#include "Swapchain.hpp"

namespace Vulkan
{

    void Swapchain::CreateImages()
    {
        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(device.GetHandle(), handle, &imageCount, nullptr);
        images.resize(imageCount);
        vkGetSwapchainImagesKHR(device.GetHandle(), handle, &imageCount, images.data());
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

    VkFormat Swapchain::GetImageFormat() const
    {
        return imageFormat;
    }

    VkExtent2D Swapchain::GetExtent() const
    {
        return extent;
    }

    std::vector<VkImageView> Swapchain::GetImageViews() const
    {
        return imageViews;
    }

    VkSwapchainKHR Swapchain::GetHandle() const
    {
        return handle;
    }

    SwapchainBuilder::SwapchainBuilder(Device device) : device(device)
    {
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


    Swapchain SwapchainBuilder::Build()
    {
        Vulkan::SurfaceSupportDetails swapChainSupport = device.physicalDevice.GetSurfaceSupportDetails();

        VkSurfaceFormatKHR surfaceFormat = FindSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = FindPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = GetExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = device.physicalDevice.surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t graphicsIndex = device.FindQueueIndex(Vulkan::QueueType::Graphics);
        uint32_t presentIndex = device.FindQueueIndex(Vulkan::QueueType::Present);

        uint32_t queueFamilyIndices[] = {graphicsIndex, presentIndex};

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

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        Swapchain swapchain;
        swapchain.device = device;

        if (vkCreateSwapchainKHR(device.GetHandle(), &createInfo, nullptr, &swapchain.handle) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        swapchain.imageFormat = surfaceFormat.format;
        swapchain.extent = extent;

        swapchain.CreateImageViews();
        
        return swapchain;
    }

    VkSurfaceFormatKHR SwapchainBuilder::FindSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
    {
        for (const auto &availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR SwapchainBuilder::FindPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
    {
        for (const auto &availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapchainBuilder::GetExtent(const VkSurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D extent = {static_cast<uint32_t>(desiredWidth), static_cast<uint32_t>(desiredHeight)};

            extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return extent;
        }
    }

} // namespace Vulkan