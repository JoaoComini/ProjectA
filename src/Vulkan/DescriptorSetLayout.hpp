#pragma once

#include <vulkan/vulkan.h>

#include "Resource.hpp"
#include "Device.hpp"

namespace Vulkan
{
	class DescriptorSetLayout : public Resource<VkDescriptorSetLayout>
	{
	public:
		DescriptorSetLayout(const Device& device, std::vector<VkDescriptorSetLayoutBinding> bindings);
		~DescriptorSetLayout();

		const std::vector<VkDescriptorSetLayoutBinding>& GetBindings() const;

		const VkDescriptorSetLayoutBinding* GetBinding(uint32_t binding) const;

	private:
		const Device& device;
		std::vector<VkDescriptorSetLayoutBinding> bindings;
	};
};