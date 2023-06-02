#include "PipelineLayout.hpp"

#include "Device.hpp"

namespace Vulkan
{
	PipelineLayout::PipelineLayout(const Device& device, const std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts)
		: device(device)
	{
		std::vector<VkDescriptorSetLayout> handles(descriptorSetLayouts.size());
		std::transform(descriptorSetLayouts.begin(), descriptorSetLayouts.end(), handles.begin(), [](auto& layout) { return layout->GetHandle(); });

		VkPipelineLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.setLayoutCount = handles.size();
		createInfo.pSetLayouts = handles.data();
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