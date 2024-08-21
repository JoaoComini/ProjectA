#pragma once

#include <vk_mem_alloc.h>

#include "Resource.hpp"
#include "PhysicalDevice.hpp"
#include "Details.hpp"
#include "Queue.hpp"
#include "CommandPool.hpp"
#include "Buffer.hpp"
#include "Image.hpp"

namespace Vulkan
{
	class ResourceCache;

	class Device : public Resource<VkDevice>
	{
	public:
		Device(const Instance& instance, const PhysicalDevice& physicalDevice);
		~Device();

		void WaitIdle() const;

		void OneTimeSubmit(std::function<void(CommandBuffer&)> func);

		void ResetCommandPool();

		Queue& GetPresentQueue() const;
		Queue& GetGraphicsQueue() const;

		uint32_t GetGraphicsQueueFamilyIndex() const;
		uint32_t GetPresentQueueFamilyIndex() const;

		VmaAllocator GetAllocator() const;

		SurfaceSupportDetails GetSurfaceSupportDetails() const;
		VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const;

		VkSampleCountFlagBits GetMaxSampleCount() const;

		ResourceCache& GetResourceCache() const;
 
	private:
		std::unique_ptr<CommandPool> commandPool;

		VmaAllocator allocator;

		std::unique_ptr<Queue> presentQueue;
		std::unique_ptr<Queue> graphicsQueue;

		const PhysicalDevice& physicalDevice;

		std::unique_ptr<ResourceCache> resourceCache;

		friend class SwapchainBuilder;
	};
}
