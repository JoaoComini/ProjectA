#pragma once

#include <vulkan/vulkan.h>

#include "Resource.hpp"
#include "Device.hpp"
#include "DescriptorSetLayout.hpp"

namespace Vulkan
{
	class DescriptorPool : Resource<VkDescriptorPool>
	{
	public:
		DescriptorPool(const Device& device, const DescriptorSetLayout& descriptorSetLayout, uint32_t size);
		~DescriptorPool();

		VkDescriptorSet Allocate();
		void Reset();

	private:
		const Device& device;
		const DescriptorSetLayout& descriptorSetLayout;
	};
};