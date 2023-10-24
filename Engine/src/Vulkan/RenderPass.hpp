#pragma once

#include <vulkan/vulkan.h>

#include "Common/Hash.hpp"

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

namespace std
{
	template <>
	struct hash<Vulkan::RenderPass>
	{
		size_t operator()(const Vulkan::RenderPass& renderPass) const
		{
			return Hash(renderPass.GetHandle());
		}
	};
};