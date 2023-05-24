#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Resource.hpp"

namespace Vulkan
{
	class Device;

	class PipelineLayout : public Resource<VkPipelineLayout>
	{
	public:
		PipelineLayout(const Device& device, std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
		~PipelineLayout();

	private:
		const Device& device;
	};
}