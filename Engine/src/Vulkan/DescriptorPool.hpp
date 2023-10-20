#pragma once

#include <vulkan/vulkan.h>

#include "Resource.hpp"
#include "Device.hpp"
#include "DescriptorSetLayout.hpp"

#include <vector>

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