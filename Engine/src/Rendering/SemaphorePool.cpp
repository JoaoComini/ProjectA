#include "SemaphorePool.hpp"

namespace Engine
{
	SemaphorePool::SemaphorePool(const Vulkan::Device& device) : device(device)
	{
	}

	Vulkan::Semaphore& SemaphorePool::RequestSemaphore()
	{
		if (activeSemaphoresCount < semaphores.size())
		{
			return *semaphores[activeSemaphoresCount++];
		}

		semaphores.emplace_back(std::make_unique<Vulkan::Semaphore>(device));

		activeSemaphoresCount++;

		return *semaphores.back();
	}

	void SemaphorePool::Reset()
	{
		activeSemaphoresCount = 0;
	}
}
