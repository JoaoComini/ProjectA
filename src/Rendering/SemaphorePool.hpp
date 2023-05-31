#pragma once

#include <vector>

#include "Vulkan/Semaphore.hpp"
#include "Vulkan/Device.hpp"

namespace Rendering
{
	class SemaphorePool
	{
	public:
		SemaphorePool(const Vulkan::Device &device);
		~SemaphorePool() = default;

		Vulkan::Semaphore& RequestSemaphore();
		void Reset();

	private:
		const Vulkan::Device& device;

		std::vector<std::unique_ptr<Vulkan::Semaphore>> semaphores;

		uint32_t activeSemaphoresCount = 0;
	};
}