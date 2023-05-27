#include "DescriptorSetLayout.hpp"

namespace Vulkan
{
	DescriptorSetLayout::DescriptorSetLayout(const Device& device, std::vector<VkDescriptorSetLayoutBinding> bindings)
		: device(device), bindings(bindings)
	{
		VkDescriptorSetLayoutCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(bindings.size()),
			.pBindings = bindings.data()
		};

		vkCreateDescriptorSetLayout(device.GetHandle(), &createInfo, nullptr, &handle);
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(device.GetHandle(), handle, nullptr);
	}

	const std::vector<VkDescriptorSetLayoutBinding>& DescriptorSetLayout::GetBindings() const
	{
		return bindings;
	}
};