#pragma once

#include "Resource.hpp"

namespace Vulkan
{
	class Device;
	class CommandBuffer;

	class CommandPool : public Resource<VkCommandPool>
	{
	public:
		CommandPool(const Device &device);
		~CommandPool();

		CommandBuffer& RequestCommandBuffer();

		void Reset();

	private:
		const Device& device;

		std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;

		uint32_t activeCommandBuffersCount = 0;
	};

}