#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

#include "Device.hpp"
#include "RenderPass.hpp"
#include "ImageView.hpp"
#include "Resource.hpp"

namespace Vulkan
{
	class Framebuffer: public Resource<VkFramebuffer>
	{
	public:
		Framebuffer(const Device& device, const RenderPass& renderPass, std::vector<std::unique_ptr<ImageView>>& attachments, VkExtent2D extent);
		~Framebuffer();

	private:
		const Device& device;

	};
}