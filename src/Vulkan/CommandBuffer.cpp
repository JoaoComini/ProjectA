#include "CommandBuffer.hpp"


namespace Vulkan
{
	CommandBuffer::CommandBuffer(const Device& device)
	{
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = device.GetCommandPool();
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(device.GetHandle(), &allocateInfo, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	const CommandBuffer& CommandBuffer::Begin()
	{
		VkCommandBufferBeginInfo beginInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		};

		vkBeginCommandBuffer(handle, &beginInfo);

		return *this;
	}

	const CommandBuffer& CommandBuffer::BeginRenderPass()
	{
		return *this;
	}

}
