#pragma once

#include <vulkan/vulkan.h>

#include "Device.hpp"
#include "Swapchain.hpp"
#include "Resource.hpp"

namespace Vulkan
{
	class RenderPass : public Resource<VkRenderPass>
	{
	public:
		RenderPass(const Device& device, const Swapchain& swapchain);
		~RenderPass();

	private:
		const Device& device;

	};
};