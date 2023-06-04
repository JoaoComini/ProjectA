#include "Device.hpp"

#include "CommandBuffer.hpp"

namespace Vulkan
{
	Device::Device(const Instance& instance, const PhysicalDevice& physicalDevice) : physicalDevice(physicalDevice)
	{
		uint32_t graphicsQueueFamilyIndex = physicalDevice.FindQueueIndex(Queue::Type::GRAPHICS);
		uint32_t presentQueueFamilyIndex = physicalDevice.FindQueueIndex(Queue::Type::PRESENT);

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

		VkPhysicalDeviceFeatures deviceFeatures{
			.samplerAnisotropy = VK_TRUE,
		};

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

	void Device::CopyBuffer(const Buffer& src, const Buffer& dst, uint32_t size)
	{
		CommandBuffer& commandBuffer = commandPool->RequestCommandBuffer();

		commandBuffer.Begin(CommandBuffer::BeginFlags::OneTimeSubmit);
		commandBuffer.CopyBuffer(src.GetHandle(), dst.GetHandle(), size);
		commandBuffer.End();

		graphicsQueue->Submit(commandBuffer);
		graphicsQueue->WaitIdle();
	}

	void Device::CopyBufferToImage(const Buffer& src, const Image& dest, uint32_t width, uint32_t height) const
	{
		CommandBuffer& commandBuffer = commandPool->RequestCommandBuffer();

		commandBuffer.Begin(CommandBuffer::BeginFlags::OneTimeSubmit);
		commandBuffer.CopyBufferToImage(src.GetHandle(), dest.GetHandle(), width, height);
		commandBuffer.End();

		graphicsQueue->Submit(commandBuffer);
		graphicsQueue->WaitIdle();
	}

	void Device::SetImageLayout(const Image& image, VkImageLayout oldLayout, VkImageLayout newLayout) const
	{
		CommandBuffer& commandBuffer = commandPool->RequestCommandBuffer();

		commandBuffer.Begin(CommandBuffer::BeginFlags::OneTimeSubmit);
		commandBuffer.SetImageLayout(image, oldLayout, newLayout, 0, image.GetMipLevels());
		commandBuffer.End();

		graphicsQueue->Submit(commandBuffer);
		graphicsQueue->WaitIdle();
	}

	void Device::GenerateMipMaps(const Image& image) const
	{
		CommandBuffer& commandBuffer = commandPool->RequestCommandBuffer();

		commandBuffer.Begin(CommandBuffer::BeginFlags::OneTimeSubmit);
		commandBuffer.GenerateMipMaps(image);
		commandBuffer.End();

		graphicsQueue->Submit(commandBuffer);
		graphicsQueue->WaitIdle();
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

	VkPhysicalDeviceProperties Device::GetPhysicalDeviceProperties() const
	{
		return physicalDevice.GetProperties();
	}

	VkSampleCountFlagBits Device::GetMaxSampleCount() const
	{
		auto properties = GetPhysicalDeviceProperties();

		auto counts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;

		std::vector<VkSampleCountFlagBits> priorities = {
			VK_SAMPLE_COUNT_64_BIT,
			VK_SAMPLE_COUNT_32_BIT,
			VK_SAMPLE_COUNT_16_BIT,
			VK_SAMPLE_COUNT_8_BIT,
			VK_SAMPLE_COUNT_4_BIT,
			VK_SAMPLE_COUNT_2_BIT,
			VK_SAMPLE_COUNT_1_BIT,
		};

		return *std::find_if(priorities.begin(), priorities.end(), [counts](auto& count) { return count & counts; });
	}
}
