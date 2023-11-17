#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Resource.hpp"

#include <memory>

namespace Vulkan
{
	class Device;

	class Image : public Resource<VkImage>
	{
	public:
		Image(
			const Device& device,
			VkImageUsageFlags usage,
			VkFormat format,
			VkExtent3D extent,
			VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
			uint32_t mipLevels = 1,
			uint32_t arrayLayers = 1,
			VkImageCreateFlags flags = 0
		);
		Image(const Device& device, VkImage handle, VkImageUsageFlags usage, VkFormat format, VkExtent3D extent);
		~Image();

		VkImageUsageFlags GetUsage() const;
		VkFormat GetFormat() const;
		VkExtent3D GetExtent() const;
		VkSampleCountFlagBits GetSampleCount() const;
		uint32_t GetMipLevels() const;
		uint32_t GetArrayLayers() const;

	private:
		VmaAllocation allocation = VK_NULL_HANDLE;
		VkImageUsageFlags usage;
		VkFormat format;
		VkExtent3D extent;
		VkSampleCountFlagBits sampleCount;
		uint32_t mipLevels;
		uint32_t arrayLayers;

		const Device& device;
	};

	class ImageBuilder
	{
	public:
		ImageBuilder& Usage(VkImageUsageFlags usage);
		ImageBuilder& Format(VkFormat format);
		ImageBuilder& Extent(VkExtent3D extent);
		ImageBuilder& SampleCount(VkSampleCountFlagBits sampleCount);
		ImageBuilder& MipLevels(uint32_t mipLevels);
		ImageBuilder& ArrayLayers(uint32_t arrayLayers);
		ImageBuilder& Flags(VkImageCreateFlags flags);
		
		std::unique_ptr<Image> Build(const Device& device);

	private:
		VkImageUsageFlags usage{ VK_IMAGE_USAGE_SAMPLED_BIT };
		VkFormat format{ VK_FORMAT_R8G8B8A8_UNORM };
		VkExtent3D extent{};
		VkSampleCountFlagBits sampleCount{ VK_SAMPLE_COUNT_1_BIT };
		uint32_t mipLevels{ 1 };
		uint32_t arrayLayers{ 1 };
		VkImageCreateFlags flags{ 0 };
	};

} // namespace Vulkan
