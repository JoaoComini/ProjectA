#include "Frame.hpp"

namespace Rendering
{

	Frame::Frame(Vulkan::Device& device): device(device)
	{
		commandPool = std::make_unique<Vulkan::CommandPool>(device);
		semaphorePool = std::make_unique<Vulkan::SemaphorePool>(device);
		renderFence = std::make_unique<Vulkan::Fence>(device);
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
		semaphorePool->Reset();
	}

	Vulkan::CommandBuffer& Frame::RequestCommandBuffer()
	{
		return commandPool->RequestCommandBuffer();
	}

	Vulkan::Semaphore& Frame::RequestSemaphore()
	{
		return semaphorePool->RequestSemaphore();
	}
	
}