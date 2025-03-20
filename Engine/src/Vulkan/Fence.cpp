#include "Fence.h"


namespace Vulkan
{
	Fence::Fence(const Device& device, bool signaled) : device(device)
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

		if (vkCreateFence(device.GetHandle(), &fenceInfo, nullptr, &handle) != VK_SUCCESS)
		{

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}

	Fence::~Fence()
	{
		vkDestroyFence(device.GetHandle(), handle, nullptr);
	}

	void Fence::Wait()
	{
		vkWaitForFences(device.GetHandle(), 1, &handle, VK_TRUE, UINT64_MAX);
	}

	void Fence::Reset()
	{
		vkResetFences(device.GetHandle(), 1, &handle);
	}
}