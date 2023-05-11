#pragma once

#include "Vulkan/Semaphore.hpp"
#include "Vulkan/Fence.hpp"
#include "Vulkan/CommandPool.hpp"
#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/Device.hpp"

namespace Rendering
{
	class Frame
	{
	public:
		Frame(Vulkan::Device& device);
		~Frame() = default;

		Vulkan::Semaphore& GetAcquireSemaphore() const;
		Vulkan::Semaphore& GetRenderFinishedSemaphore() const;
		Vulkan::Fence& GetRenderFence() const;

		void Reset();
		Vulkan::CommandBuffer& RequestCommandBuffer();

	private:
		Vulkan::Device& device;

		std::unique_ptr<Vulkan::CommandPool> commandPool;
		std::unique_ptr<Vulkan::Semaphore> acquireSemaphore;
		std::unique_ptr<Vulkan::Semaphore> renderFinishedSemaphore;
		std::unique_ptr<Vulkan::Fence> renderFence;
	};
}