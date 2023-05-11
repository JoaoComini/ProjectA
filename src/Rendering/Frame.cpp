#include "Frame.hpp"

namespace Rendering
{

	Frame::Frame(Vulkan::Device& device): device(device)
	{
		commandPool = std::make_unique<Vulkan::CommandPool>(device);
		acquireSemaphore = std::make_unique<Vulkan::Semaphore>(device);
		renderFinishedSemaphore = std::make_unique<Vulkan::Semaphore>(device);
		renderFence = std::make_unique<Vulkan::Fence>(device);
	}

	Vulkan::Semaphore& Frame::GetAcquireSemaphore() const
	{
		return *acquireSemaphore;
	}

	Vulkan::Semaphore& Frame::GetRenderFinishedSemaphore() const
	{
		return *renderFinishedSemaphore;
	}

	Vulkan::Fence& Frame::GetRenderFence() const
	{
		return *renderFence;
	}

	void Frame::Reset()
	{
		renderFence->Wait();
		renderFence->Reset();

		commandPool->Reset();
	}

	VkCommandBuffer Frame::RequestCommandBuffer()
	{
		return commandPool->RequestCommandBuffer();
	}
	
}