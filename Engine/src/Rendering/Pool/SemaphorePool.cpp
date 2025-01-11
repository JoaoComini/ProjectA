#include "SemaphorePool.hpp"

namespace Engine
{
	SemaphorePool::SemaphorePool(const Vulkan::Device& device) : device(device)
	{
	}

	SemaphorePool::~SemaphorePool()
	{
		for (auto semaphore : released)
		{
			delete semaphore;
		}
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

	Vulkan::Semaphore* SemaphorePool::RequestOwnedSemaphore()
	{
		if (activeSemaphoresCount < semaphores.size())
		{
			auto& semaphore = semaphores.back();

			auto owned = semaphore.release();

			semaphores.pop_back();

			return owned;
		}

		return new Vulkan::Semaphore(device);
	}

	void SemaphorePool::ReleaseOwnedSemaphore(Vulkan::Semaphore* semaphore)
	{
		released.push_back(semaphore);
	}

	void SemaphorePool::Reset()
	{
		activeSemaphoresCount = 0;

		for (auto semaphore : released)
		{
			semaphores.emplace_back(std::unique_ptr<Vulkan::Semaphore>(semaphore));
		}

		released.clear();
	}
}
