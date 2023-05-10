#pragma once

#include <vulkan/vulkan.h>

#include "Vulkan/Semaphore.hpp"
#include "Vulkan/CommandPool.hpp"
#include "Vulkan/Device.hpp"

namespace Rendering
{
	class Frame
	{
	public:
		Frame(Vulkan::Device& device);
		~Frame();

		Vulkan::Semaphore& GetAcquireSemaphore() const;
		Vulkan::Semaphore& GetRenderFinishedSemaphore() const;
		VkFence GetRenderFence() const;

		void Reset();
		VkCommandBuffer RequestCommandBuffer();

	private:
		Vulkan::CommandPool& GetThreadCommandPool();

		Vulkan::Device& device;

		std::unique_ptr<Vulkan::Semaphore> acquireSemaphore;
		std::unique_ptr<Vulkan::Semaphore> renderFinishedSemaphore;
		std::vector<std::unique_ptr<Vulkan::CommandPool>> commandPools;

		VkFence renderFence;

		std::mutex mutex;
	};
}