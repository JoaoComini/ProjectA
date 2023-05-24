#include "PipelineLayout.hpp"

#include "Device.hpp"

namespace Vulkan
{
	PipelineLayout::PipelineLayout(const Device& device, std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
		: device(device)
	{
		VkPipelineLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.setLayoutCount = descriptorSetLayouts.size();
		createInfo.pSetLayouts = descriptorSetLayouts.data();
		createInfo.pushConstantRangeCount = 0;
		createInfo.pPushConstantRanges = VK_NULL_HANDLE;

		if (vkCreatePipelineLayout(device.GetHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	PipelineLayout::~PipelineLayout()
	{
		vkDestroyPipelineLayout(device.GetHandle(), handle, nullptr);
	}
}