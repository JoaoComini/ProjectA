#include "CommandBuffer.hpp"

#include "Device.hpp"
#include "CommandPool.hpp"

namespace Vulkan
{
	CommandBuffer::CommandBuffer(const Device& device, const CommandPool& commandPool, const Level level) : device(device), commandPool(commandPool)
	{
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = commandPool.GetHandle();
		allocateInfo.level = level == Level::Primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocateInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(device.GetHandle(), &allocateInfo, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	CommandBuffer::~CommandBuffer()
	{
		if (handle == VK_NULL_HANDLE)
		{
			return;
		}

		vkFreeCommandBuffers(device.GetHandle(), commandPool.GetHandle(), 1, &handle);
	}

	void CommandBuffer::Begin(BeginFlags flags)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = static_cast<VkFlags>(flags);                  // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(handle, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}

	void CommandBuffer::End()
	{
		vkEndCommandBuffer(handle);
	}

	void CommandBuffer::Free()
	{
		vkFreeCommandBuffers(device.GetHandle(), commandPool.GetHandle(), 1, &handle);
	}

	void CommandBuffer::CopyBuffer(VkBuffer src, VkBuffer dst, uint32_t size)
	{
		VkBufferCopy copy{
			.size = size
		};

		vkCmdCopyBuffer(handle, src, dst, 1, &copy);
	}
}
