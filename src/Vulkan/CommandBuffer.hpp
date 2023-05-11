#pragma once

#include <vulkan/vulkan.h>

#include "Device.hpp"
#include "Resource.hpp"

namespace Vulkan
{
	class CommandPool;

	class CommandBuffer: public Resource<VkCommandBuffer>
	{
	public:
		CommandBuffer(const Device& device, const CommandPool& commandPool);
		~CommandBuffer();

		void Begin();

	private:
		const Device& device;
		const CommandPool& commandPool;
	};
}
