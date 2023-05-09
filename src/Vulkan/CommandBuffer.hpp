#pragma once


#include <vulkan/vulkan.h>
#include <Vulkan/Device.hpp>

namespace Vulkan
{
	class CommandBuffer
	{
	public:
		CommandBuffer(const Device& device);

		const CommandBuffer& Begin();
		const CommandBuffer& BeginRenderPass();

	private:
		VkCommandBuffer handle;
	};
}
