#pragma once

#include <vulkan/vulkan.h>

#include "Device.hpp"
#include "PipelineLayout.hpp"
#include "Resource.hpp"

namespace Vulkan
{

	struct VertexInputSpec
	{
		std::vector<VkVertexInputAttributeDescription> attributes;
		std::vector<VkVertexInputBindingDescription> bindings;
	};

	struct PipelineSpec
	{
		VertexInputSpec vertexInputSpec;
	};

	class Pipeline : public Resource<VkPipeline>
	{
	public:
		Pipeline(const Device& device, const PipelineLayout& layout, VkRenderPass renderPass, PipelineSpec spec);
		~Pipeline();

	private:
		const Device& device;
	};
};