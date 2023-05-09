#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Device.hpp"
#include "Resource.hpp"

namespace Vulkan
{
	class Image : public Resource<VkImage>
	{
	public:
		Image(const Device& device, uint32_t width, uint32_t height);
		~Image();

		VkImageView GetView() const;

	private:
		VkImageView view;
		VmaAllocation allocation;

		const Device& device;
	};

} // namespace Vulkan
