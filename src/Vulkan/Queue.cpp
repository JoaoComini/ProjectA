#include "Queue.hpp"

#include "Device.hpp"

namespace Vulkan
{
	Queue::Queue(const Device& device, uint32_t familyIndex): device(device), familyIndex(familyIndex)
	{
		vkGetDeviceQueue(device.GetHandle(), familyIndex, 0, &handle);
	}

	void Queue::Submit(const CommandBuffer& commandBuffer)
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer.GetHandle();
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