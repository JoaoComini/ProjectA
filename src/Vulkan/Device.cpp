#include "Device.hpp"

#include "CommandBuffer.hpp"

namespace Vulkan
{
	Device::Device(const Instance& instance, const PhysicalDevice& physicalDevice) : physicalDevice(physicalDevice)
	{
		uint32_t graphicsQueueFamilyIndex = physicalDevice.FindQueueIndex(Queue::Type::Graphics);
		uint32_t presentQueueFamilyIndex = physicalDevice.FindQueueIndex(Queue::Type::Present);

		std::set<uint32_t> familyIndices = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		float queuePriority = 1.0f;
		for (uint32_t index : familyIndices)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = index;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		const std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		VkDeviceCreateInfo deviceCreateinfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledLayerCount = 0,
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
			.pEnabledFeatures = &deviceFeatures,
		};

		if (vkCreateDevice(physicalDevice.handle, &deviceCreateinfo, nullptr, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}

		graphicsQueue = std::make_unique<Queue>(*this, graphicsQueueFamilyIndex);
		presentQueue = std::make_unique<Queue>(*this, presentQueueFamilyIndex);

		const VmaAllocatorCreateInfo allocatorCreateInfo{
			.physicalDevice = physicalDevice.handle,
			.device = handle,
			.instance = instance.GetHandle(),
			.vulkanApiVersion = VK_API_VERSION_1_3,
		};

		if (const auto result = vmaCreateAllocator(&allocatorCreateInfo, &allocator); result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vmaCreateAllocator!");
		}

		commandPool = std::make_unique<CommandPool>(*this);
	}

	Device::~Device()
	{
		commandPool.reset();

		vmaDestroyAllocator(allocator);
		vkDestroyDevice(handle, nullptr);
	}

	void Device::WaitIdle() const
	{
		vkDeviceWaitIdle(handle);
	}

	void Device::CopyBuffer(VkBuffer src, VkBuffer dst, uint32_t size)
	{
		CommandBuffer& commandBuffer = commandPool->RequestCommandBuffer();

		commandBuffer.Begin(CommandBuffer::BeginFlags::OneTimeSubmit);
		commandBuffer.CopyBuffer(src, dst, size);
		commandBuffer.End();

		graphicsQueue->Submit(commandBuffer);

		graphicsQueue->WaitIdle();

		commandBuffer.Free();
	}

	Queue& Device::GetGraphicsQueue() const
	{
		return *graphicsQueue;
	}

	Queue& Device::GetPresentQueue() const
	{
		return *presentQueue;
	}

	uint32_t Device::GetGraphicsQueueFamilyIndex() const
	{
		return graphicsQueue->GetFamilyIndex();
	}

	uint32_t Device::GetPresentQueueFamilyIndex() const
	{
		return presentQueue->GetFamilyIndex();
	}

	VmaAllocator Device::GetAllocator() const
	{
		return allocator;
	}

	SurfaceSupportDetails Device::GetSurfaceSupportDetails() const
	{
		return physicalDevice.GetSurfaceSupportDetails();
	}
}
