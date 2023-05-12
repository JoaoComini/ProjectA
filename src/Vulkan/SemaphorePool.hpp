#pragma once

#include <vector>

#include "Semaphore.hpp"


namespace Vulkan
{
	class SemaphorePool
	{
	public:
		SemaphorePool(const Device &device);
		~SemaphorePool() = default;

		Semaphore& RequestSemaphore();
		void Reset();

	private:
		const Device& device;

		std::vector<std::unique_ptr<Semaphore>> semaphores;

		uint32_t activeSemaphoresCount = 0;
	};
}