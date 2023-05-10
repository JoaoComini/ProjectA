#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Resource.hpp"

namespace Vulkan
{
	class Device;

	class CommandPool : public Resource<VkCommandPool>
	{
	public:
		CommandPool(Device &device);
		~CommandPool();

		VkCommandBuffer RequestCommandBuffer();

		void Reset();

	private:
		Device& device;

		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t activeCommandBuffersCount = 0;
	};

}