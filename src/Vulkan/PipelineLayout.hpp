#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Resource.hpp"
#include "DescriptorSetLayout.hpp"

namespace Vulkan
{
	class Device;

	class PipelineLayout : public Resource<VkPipelineLayout>
	{
	public:
		PipelineLayout(const Device& device, const std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts);
		~PipelineLayout();

	private:
		const Device& device;
	};
}