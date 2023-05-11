#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <vector>
#include <mutex>
#include <set>
#include <stdexcept>

#include "Resource.hpp"
#include "PhysicalDevice.hpp"
#include "Details.hpp"
#include "Queue.hpp"
#include "CommandPool.hpp"

namespace Vulkan
{
	class Device : public Resource<VkDevice>
	{
	public:
		Device(const Instance& instance, const PhysicalDevice& physicalDevice);
		~Device();

		void CopyBuffer(VkBuffer src, VkBuffer dest, uint32_t size);
		void WaitIdle();

		Queue& GetPresentQueue() const;
		Queue& GetGraphicsQueue() const;

		uint32_t GetGraphicsQueueFamilyIndex() const;
		uint32_t GetPresentQueueFamilyIndex() const;

		VmaAllocator GetAllocator() const;

		SurfaceSupportDetails GetSurfaceSupportDetails() const;

	private:
		std::unique_ptr<CommandPool> commandPool;

		VmaAllocator allocator;

		std::unique_ptr<Queue> presentQueue;
		std::unique_ptr<Queue> graphicsQueue;

		const PhysicalDevice& physicalDevice;

		friend class SwapchainBuilder;
	};
}
