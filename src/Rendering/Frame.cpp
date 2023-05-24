#include "Frame.hpp"

namespace Rendering
{

	Frame::Frame(const Vulkan::Device& device, std::unique_ptr<Target> target): device(device), target(std::move(target))
	{
		commandPool = std::make_unique<Vulkan::CommandPool>(device);
		semaphorePool = std::make_unique<Vulkan::SemaphorePool>(device);
		renderFence = std::make_unique<Vulkan::Fence>(device);

		uniformBuffer = Vulkan::BufferBuilder()
			.Persistent()
			.AllowTransfer()
			.SequentialWrite()
			.BufferUsage(Vulkan::BufferUsageFlags::UNIFORM)
			.Size(sizeof(GlobalUniform))
			.Build(device);
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

	void Frame::SetTarget(std::unique_ptr<Target> target)
	{
		this->target = std::move(target);
	}

	Target& Frame::GetTarget() const
	{
		return *target;
	}
}