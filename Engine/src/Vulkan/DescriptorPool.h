#pragma once

#include "Resource.h"
#include "Device.h"
#include "DescriptorSetLayout.h"

namespace Vulkan
{
	class DescriptorPool : public Resource<VkDescriptorPool>
	{
	public:
		DescriptorPool(const Device& device, DescriptorSetLayout& descriptorSetLayout, uint32_t size);
		DescriptorPool(const Device& device, std::vector<VkDescriptorPoolSize> poolSizes, uint32_t size);

		~DescriptorPool();

		VkDescriptorSet Allocate();
		void Reset();

	private:
		const Device& device;
		DescriptorSetLayout* descriptorSetLayout;
	};
};