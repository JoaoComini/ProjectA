#include "DescriptorPool.h"


namespace Engine
{
	DescriptorPool::DescriptorPool(const Vulkan::Device& device, Vulkan::DescriptorSetLayout& descriptorSetLayout, uint32_t size)
		: device(device), descriptorSetLayout(descriptorSetLayout), size(size)
	{
		entries.emplace_back(std::make_unique<DescriptorPoolEntry>(device, descriptorSetLayout, size));
	}
	
	VkDescriptorSet DescriptorPool::Allocate()
	{
		auto& activeEntry = entries[activeEntryIndex];

		if (activeEntry->CanAllocate())
		{
			return activeEntry->Allocate();
		}

		activeEntryIndex++;

		if (entries.size() <= activeEntryIndex)
		{
			entries.emplace_back(std::make_unique<DescriptorPoolEntry>(device, descriptorSetLayout, size));
		}

		return entries[activeEntryIndex]->Allocate();
	}

	void DescriptorPool::Reset()
	{
		for (auto& entry : entries)
		{
			entry->Reset();
		}

		activeEntryIndex = 0;
	}

	DescriptorPoolEntry::DescriptorPoolEntry(const Vulkan::Device& device, Vulkan::DescriptorSetLayout& descriptorSetLayout, uint32_t size)
		: size(size)
	{
		pool = std::make_unique<Vulkan::DescriptorPool>(device, descriptorSetLayout, size);
	}

	bool DescriptorPoolEntry::CanAllocate()
	{
		return setsCount < size;
	}

	VkDescriptorSet DescriptorPoolEntry::Allocate()
	{
		setsCount++;
		return pool->Allocate();
	}

	void DescriptorPoolEntry::Reset()
	{
		pool->Reset();
		setsCount = 0;
	}
}
