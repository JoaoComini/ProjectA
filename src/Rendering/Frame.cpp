#include "Frame.hpp"

namespace Rendering
{

	Frame::Frame(Vulkan::Device& device): device(device)
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateFence(device.GetHandle(), &fenceInfo, nullptr, &renderFence) != VK_SUCCESS)
		{

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}

		acquireSemaphore = std::make_unique<Vulkan::Semaphore>(device);
		renderFinishedSemaphore = std::make_unique<Vulkan::Semaphore>(device);
	}

	Frame::~Frame()
	{
		vkDestroyFence(device.GetHandle(), renderFence, nullptr);
	}

	Vulkan::Semaphore& Frame::GetAcquireSemaphore() const
	{
		return *acquireSemaphore;
	}

	Vulkan::Semaphore& Frame::GetRenderFinishedSemaphore() const
	{
		return *renderFinishedSemaphore;
	}

	VkFence Frame::GetRenderFence() const
	{
		return renderFence;
	}

	void Frame::Reset()
	{
		vkWaitForFences(device.GetHandle(), 1, &renderFence, VK_TRUE, UINT64_MAX);

		vkResetFences(device.GetHandle(), 1, &renderFence);
		GetThreadCommandPool().Reset();
	}

	VkCommandBuffer Frame::RequestCommandBuffer()
	{
		return GetThreadCommandPool().RequestCommandBuffer();
	}

	Vulkan::CommandPool& Frame::GetThreadCommandPool()
	{
		thread_local Vulkan::CommandPool* threadCommandPool = nullptr;

		if (threadCommandPool == nullptr) {
			threadCommandPool = commandPools.emplace_back(std::make_unique<Vulkan::CommandPool>(device)).get();
		}

		return *threadCommandPool;
	}

	
}