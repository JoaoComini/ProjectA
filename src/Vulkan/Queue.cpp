#include "Queue.hpp"

#include "Device.hpp"
#include "CommandBuffer.hpp"
#include "Semaphore.hpp"
#include "Fence.hpp"
#include "Swapchain.hpp"

namespace Vulkan
{
	Queue::Queue(const Device& device, uint32_t familyIndex) : device(device), familyIndex(familyIndex)
	{
		vkGetDeviceQueue(device.GetHandle(), familyIndex, 0, &handle);
	}

	void Queue::Submit(const CommandBuffer& commandBuffer)
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer.GetHandle();

		vkQueueSubmit(handle, 1, &submitInfo, VK_NULL_HANDLE);
	}

	void Queue::Submit(const CommandBuffer& commandBuffer, const Semaphore& waitSemaphore, const Semaphore& signalSemaphore, const Fence& fence)
	{
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo info{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &waitSemaphore.GetHandle(),
			.pWaitDstStageMask = waitStages,
			.commandBufferCount = 1,
			.pCommandBuffers = &commandBuffer.GetHandle(),
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &signalSemaphore.GetHandle(),
		};

		if (vkQueueSubmit(handle, 1, &info, fence.GetHandle()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}
	}

	VkResult Queue::Present(const Swapchain& swapchain, const Semaphore& waitSemaphore, uint32_t imageIndex)
	{
		VkPresentInfoKHR info{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &waitSemaphore.GetHandle(),
			.swapchainCount = 1,
			.pSwapchains = &swapchain.GetHandle(),
			.pImageIndices = &imageIndex,
		};

		return vkQueuePresentKHR(handle, &info);
	}

	void Queue::WaitIdle()
	{
		vkQueueWaitIdle(handle);
	}

	uint32_t Queue::GetFamilyIndex() const
	{
		return familyIndex;
	}
}