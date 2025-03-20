#pragma once

#include "Resource.h"

namespace Vulkan
{
	class Device;
	class CommandBuffer;
	class Semaphore;
	class Fence;
	class Swapchain;

	class Queue : public Resource<VkQueue>
	{
	public:
		enum Type { PRESENT, GRAPHICS };

		Queue(const Device& device, uint32_t familyIndex);
		~Queue() = default;

		void Submit(const CommandBuffer& commandBuffer);
		void Submit(const CommandBuffer& commandBuffer, const Semaphore& waitSemaphore, const Semaphore& signalSemaphore, const Fence& fence);

		VkResult Present(const Swapchain& swapchain, const Semaphore& waitSemaphore, uint32_t imageIndex);

		void WaitIdle();

		uint32_t GetFamilyIndex() const;

	private:
		const Device& device;
		uint32_t familyIndex;

	};
}
