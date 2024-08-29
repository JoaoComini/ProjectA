#pragma once

#include "Vulkan/Semaphore.hpp"
#include "Vulkan/Device.hpp"

namespace Engine
{
	class SemaphorePool
	{
	public:
		SemaphorePool(const Vulkan::Device &device);
		~SemaphorePool() = default;

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