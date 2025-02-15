#include "CommandPool.hpp"

#include "Device.hpp"
#include "CommandBuffer.hpp"

#include "Rendering/RenderFrame.hpp"

namespace Vulkan
{
	CommandPool::CommandPool(Device &device, Engine::RenderFrame* frame) : device(device), frame(frame)
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
		commandBuffers.clear();

		vkDestroyCommandPool(device.GetHandle(), handle, nullptr);
	}

	CommandBuffer& CommandPool::RequestCommandBuffer()
	{
		if (activeCommandBuffersCount < commandBuffers.size())
		{
			return *commandBuffers[activeCommandBuffersCount++];
		}

		commandBuffers.emplace_back(std::make_unique<CommandBuffer>(device, *this));

		activeCommandBuffersCount++;

		return *commandBuffers.back();
	}

	Engine::RenderFrame* CommandPool::GetRenderFrame()
	{
		return frame;
	}

	void CommandPool::Reset()
	{
		vkResetCommandPool(device.GetHandle(), handle, 0);

		activeCommandBuffersCount = 0;
	}
}