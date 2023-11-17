#include "Image.hpp"

#include "Device.hpp"

namespace Vulkan
{
    Image::Image(
        const Device &device,
        VkImageUsageFlags usage,
        VkFormat format,
        VkExtent3D extent,
        VkSampleCountFlagBits samples,
        uint32_t mipLevels,
        uint32_t arrayLayers,
        VkImageCreateFlags flags
    ) : device(device), usage(usage), format(format), extent(extent), sampleCount(samples), mipLevels(mipLevels), arrayLayers(arrayLayers)
    {
        VkImageCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags = flags,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent = extent,
            .mipLevels = mipLevels,
            .arrayLayers = arrayLayers,
            .samples = samples,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        VmaAllocationCreateInfo allocationCreateInfo{
            .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO,
            .priority = 1.0f,
        };

        vmaCreateImage(device.GetAllocator(), &createInfo, &allocationCreateInfo, &handle, &allocation, nullptr);
    }

    Image::Image(const Device& device, VkImage handle, VkImageUsageFlags usage, VkFormat format, VkExtent3D extent)
        : device(device), Resource(handle), usage(usage), format(format), extent(extent), sampleCount(VK_SAMPLE_COUNT_1_BIT), mipLevels(1), arrayLayers(1)
    {
    }

    Image::~Image()
    {
        if (handle == VK_NULL_HANDLE || allocation == VK_NULL_HANDLE)
        {
            return;
        }

        vmaDestroyImage(device.GetAllocator(), handle, allocation);
    }

    VkImageUsageFlags Image::GetUsage() const
    {
        return usage;
    }

    VkExtent3D Image::GetExtent() const
    {
        return extent;
    }

    VkSampleCountFlagBits Image::GetSampleCount() const
    {
        return sampleCount;
    }

    uint32_t Image::GetMipLevels() const
    {
        return mipLevels;
    }

    VkFormat Image::GetFormat() const
    {
        return format;
    }

    uint32_t Image::GetArrayLayers() const
    {
        return arrayLayers;
    }

    ImageBuilder& ImageBuilder::Usage(VkImageUsageFlags usage)
    {
        this->usage = usage;

        return *this;
    }
    ImageBuilder& ImageBuilder::Format(VkFormat format)
    {
        this->format = format;

        return *this;
    }

    ImageBuilder& ImageBuilder::Extent(VkExtent3D extent)
    {
        this->extent = extent;

        return *this;
    }

    ImageBuilder& ImageBuilder::SampleCount(VkSampleCountFlagBits sampleCount)
    {
        this->sampleCount = sampleCount;

        return *this;
    }

    ImageBuilder& ImageBuilder::MipLevels(uint32_t mipLevels)
    {
        this->mipLevels = mipLevels;

        return *this;
    }

    ImageBuilder& ImageBuilder::ArrayLayers(uint32_t arrayLayers)
    {
        this->arrayLayers = arrayLayers;

        return *this;
    }

    ImageBuilder& ImageBuilder::Flags(VkImageCreateFlags flags)
    {
        this->flags = flags;

        return *this;
    }

    std::unique_ptr<Image> ImageBuilder::Build(const Device& device)
    {
        return std::make_unique<Image>(device, usage, format, extent, sampleCount, mipLevels, arrayLayers, flags);
    }

} // namespace Vulkan
