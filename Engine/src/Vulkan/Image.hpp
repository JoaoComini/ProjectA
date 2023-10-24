#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Resource.hpp"

namespace Vulkan
{
	class Device;

	class Image : public Resource<VkImage>
	{
	public:
		Image(const Device& device, VkImageUsageFlags usage, VkFormat format, VkExtent3D extent, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT, uint32_t mipLevels = 1);
		Image(const Device& device, VkImage handle, VkFormat format, VkExtent3D extent);
		~Image();

		VkFormat GetFormat() const;
		VkExtent3D GetExtent() const;
		uint32_t GetMipLevels() const;

	private:
		VmaAllocation allocation = VK_NULL_HANDLE;
		VkFormat format;
		VkExtent3D extent;
		uint32_t mipLevels;

		const Device& device;
	};

} // namespace Vulkan
