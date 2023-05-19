#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Resource.hpp"

namespace Vulkan
{
	class Device;

	enum class ImageUsage : uint32_t
	{
		DEPTH_STENCIL = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		SAMPLED = VK_IMAGE_USAGE_SAMPLED_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	};

	enum class ImageFormat : uint32_t
	{
		RGBA32_SRGB = VK_FORMAT_R8G8B8A8_SRGB,
		D32_SFLOAT = VK_FORMAT_D32_SFLOAT
	};

	class Image : public Resource<VkImage>
	{
	public:
		Image(const Device& device, ImageUsage usage, ImageFormat format, uint32_t width, uint32_t height);
		Image(const Device& device, VkImage handle);
		~Image();

	private:
		VmaAllocation allocation = VK_NULL_HANDLE;

		const Device& device;
	};

} // namespace Vulkan
