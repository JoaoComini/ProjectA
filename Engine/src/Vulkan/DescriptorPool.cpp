#include "DescriptorPool.hpp"



namespace Vulkan
{
	DescriptorPool::DescriptorPool(const Device& device, const DescriptorSetLayout& descriptorSetLayout, uint32_t size)
		: device(device), descriptorSetLayout(descriptorSetLayout)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings = descriptorSetLayout.GetBindings();

		std::vector<VkDescriptorPoolSize> sizes(bindings.size());
		std::transform(bindings.begin(), bindings.end(), sizes.begin(), [size](auto& binding)
			{
				return VkDescriptorPoolSize{
					.type = binding.descriptorType,
					.descriptorCount = binding.descriptorCount * size
				};
			}
		);

		VkDescriptorPoolCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = size,
			.poolSizeCount = (uint32_t)sizes.size(),
			.pPoolSizes = sizes.data(),
		};

		vkCreateDescriptorPool(device.GetHandle(), &createInfo, nullptr, &handle);
	}

	DescriptorPool::~DescriptorPool()
	{
		vkDestroyDescriptorPool(device.GetHandle(), handle, nullptr);
	}

	VkDescriptorSet DescriptorPool::Allocate()
	{
		VkDescriptorSet descriptorSetHandle = VK_NULL_HANDLE;

		VkDescriptorSetAllocateInfo allocateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = handle,
			.descriptorSetCount = 1,
			.pSetLayouts = &descriptorSetLayout.GetHandle(),
		};

		vkAllocateDescriptorSets(device.GetHandle(), &allocateInfo, &descriptorSetHandle);

		return descriptorSetHandle;
	}

	void DescriptorPool::Reset()
	{
		vkResetDescriptorPool(device.GetHandle(), handle, 0);
	}
};