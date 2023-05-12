#include "SemaphorePool.hpp"

namespace Vulkan
{
	SemaphorePool::SemaphorePool(const Device& device) : device(device)
	{
	}

	Semaphore& SemaphorePool::RequestSemaphore()
	{
		if (activeSemaphoresCount < semaphores.size())
		{
			return *semaphores[activeSemaphoresCount++];
		}

		semaphores.emplace_back(std::make_unique<Semaphore>(device));

		activeSemaphoresCount++;

		return *semaphores.back();
	}

	void SemaphorePool::Reset()
	{
		activeSemaphoresCount = 0;
	}
}
