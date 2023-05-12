#pragma once

#include "Vulkan/Semaphore.hpp"
#include "Vulkan/SemaphorePool.hpp"
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

		Vulkan::Fence& GetRenderFence() const;

		void Reset();
		Vulkan::CommandBuffer& RequestCommandBuffer();
		Vulkan::Semaphore& RequestSemaphore();

	private:
		Vulkan::Device& device;

		std::unique_ptr<Vulkan::CommandPool> commandPool;
		std::unique_ptr<Vulkan::SemaphorePool> semaphorePool;
		std::unique_ptr<Vulkan::Fence> renderFence;
	};
}