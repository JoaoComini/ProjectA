#pragma once

#include "Vulkan/DescriptorPool.h"
#include "Vulkan/Device.h"
#include "Vulkan/DescriptorSetLayout.h"

namespace Engine
{
	class DescriptorPoolEntry
	{
	public:
		DescriptorPoolEntry(const Vulkan::Device& device, Vulkan::DescriptorSetLayout& descriptorSetLayout, uint32_t size);
		~DescriptorPoolEntry() = default;

		bool CanAllocate();
		VkDescriptorSet Allocate();
		void Reset();

	private:
		std::unique_ptr<Vulkan::DescriptorPool> pool;
		uint32_t size;
		uint32_t setsCount = 0;
	};

	class DescriptorPool
	{
	public:
		DescriptorPool(const Vulkan::Device& device, Vulkan::DescriptorSetLayout& descriptorSetLayout, uint32_t size);
		~DescriptorPool() = default;

		VkDescriptorSet Allocate();
		void Reset();

	private:
		const Vulkan::Device& device;
		Vulkan::DescriptorSetLayout& descriptorSetLayout;
		uint32_t size;

		std::vector<std::unique_ptr<DescriptorPoolEntry>> entries;
		uint32_t activeEntryIndex = 0;
	};
}
