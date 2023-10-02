#include "DescriptorPool.hpp"



namespace Vulkan
{
	DescriptorPool::DescriptorPool(const Device& device, std::shared_ptr<DescriptorSetLayout> descriptorSetLayout, uint32_t size)
		: device(device), descriptorSetLayout(descriptorSetLayout)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings = descriptorSetLayout->GetBindings();

		std::vector<VkDescriptorPoolSize> poolSizes(bindings.size());
		std::transform(bindings.begin(), bindings.end(), poolSizes.begin(), [size](auto& binding)
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
			.poolSizeCount = (uint32_t)poolSizes.size(),
			.pPoolSizes = poolSizes.data(),
		};

		vkCreateDescriptorPool(device.GetHandle(), &createInfo, nullptr, &handle);
	}

	DescriptorPool::DescriptorPool(const Device& device, std::vector<VkDescriptorPoolSize> poolSizes, uint32_t size)
		: device(device)
	{
		VkDescriptorPoolCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = size,
			.poolSizeCount = (uint32_t)poolSizes.size(),
			.pPoolSizes = poolSizes.data(),
		};

		vkCreateDescriptorPool(device.GetHandle(), &createInfo, nullptr, &handle);
	}

	DescriptorPool::~DescriptorPool()
	{
		vkDestroyDescriptorPool(device.GetHandle(), handle, nullptr);
	}

	VkDescriptorSet DescriptorPool::Allocate()
	{
		if (!descriptorSetLayout)
		{
			return VK_NULL_HANDLE;
		}

		VkDescriptorSet descriptorSetHandle = VK_NULL_HANDLE;

		VkDescriptorSetAllocateInfo allocateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = handle,
			.descriptorSetCount = 1,
			.pSetLayouts = &descriptorSetLayout->GetHandle(),
		};

		vkAllocateDescriptorSets(device.GetHandle(), &allocateInfo, &descriptorSetHandle);

		return descriptorSetHandle;
	}

	void DescriptorPool::Reset()
	{
		vkResetDescriptorPool(device.GetHandle(), handle, 0);
	}
};