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
		Image(const Device& device, VkImageUsageFlags usage, VkFormat format, uint32_t width, uint32_t height);
		Image(const Device& device, VkImage handle, VkFormat format);
		~Image();

		VkFormat GetFormat() const;

	private:
		VmaAllocation allocation = VK_NULL_HANDLE;
		VkFormat format;

		const Device& device;
	};

} // namespace Vulkan
