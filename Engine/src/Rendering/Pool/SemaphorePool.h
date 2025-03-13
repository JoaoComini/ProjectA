#pragma once

#include "Vulkan/Semaphore.h"
#include "Vulkan/Device.h"

namespace Engine
{
	class SemaphorePool
	{
	public:
		SemaphorePool(const Vulkan::Device &device);
		~SemaphorePool();

		Vulkan::Semaphore& RequestSemaphore();
		Vulkan::Semaphore* RequestOwnedSemaphore();
		void ReleaseOwnedSemaphore(Vulkan::Semaphore* semaphore);
		void Reset();

	private:
		const Vulkan::Device& device;

		std::vector<std::unique_ptr<Vulkan::Semaphore>> semaphores;
		std::vector<Vulkan::Semaphore *> released;

		uint32_t activeSemaphoresCount = 0;
	};
}