#include "CommandPool.hpp"

#include "Device.hpp"


namespace Vulkan
{
	CommandPool::CommandPool(Device &device) : device(device)
	{
		VkCommandPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		poolCreateInfo.queueFamilyIndex = device.GetGraphicsQueueFamilyIndex();

		if (vkCreateCommandPool(device.GetHandle(), &poolCreateInfo, nullptr, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool!");
		}
	}

	CommandPool::~CommandPool()
	{
		vkDestroyCommandPool(device.GetHandle(), handle, nullptr);
	}

	VkCommandBuffer CommandPool::RequestCommandBuffer()
	{
		if (activeCommandBuffersCount < commandBuffers.size())
		{
			return commandBuffers[activeCommandBuffersCount++];
		}

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = handle;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device.GetHandle(), &allocInfo, &commandBuffer);

		commandBuffers.emplace_back(commandBuffer);

		activeCommandBuffersCount++;

		return commandBuffers.back();
	}

	void CommandPool::Reset()
	{
		vkResetCommandPool(device.GetHandle(), handle, 0);

		activeCommandBuffersCount = 0;
	}
}