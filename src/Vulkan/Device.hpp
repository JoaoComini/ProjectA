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
#include "Buffer.hpp"
#include "Image.hpp"

namespace Vulkan
{
	class Device : public Resource<VkDevice>
	{
	public:
		Device(const Instance& instance, const PhysicalDevice& physicalDevice);
		~Device();

		void CopyBuffer(const Buffer& src, const Buffer& dest, uint32_t size);
		void CopyBufferToImage(const Buffer& src, const Image& dest, uint32_t width, uint32_t height) const;
		void SetImageLayout(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout) const;
		void WaitIdle() const;

		Queue& GetPresentQueue() const;
		Queue& GetGraphicsQueue() const;

		uint32_t GetGraphicsQueueFamilyIndex() const;
		uint32_t GetPresentQueueFamilyIndex() const;

		VmaAllocator GetAllocator() const;

		SurfaceSupportDetails GetSurfaceSupportDetails() const;
		VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const;
 
	private:
		std::unique_ptr<CommandPool> commandPool;

		VmaAllocator allocator;

		std::unique_ptr<Queue> presentQueue;
		std::unique_ptr<Queue> graphicsQueue;

		const PhysicalDevice& physicalDevice;

		friend class SwapchainBuilder;
	};
}
