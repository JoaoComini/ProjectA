#pragma once

#include <vulkan/vulkan.h>

#include "Device.hpp"
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"
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
		std::vector<std::shared_ptr<ShaderModule>> shaderModules;
	};

	class Pipeline : public Resource<VkPipeline>
	{
	public:
		Pipeline(const Device& device, const PipelineLayout& layout, const RenderPass& renderPass, PipelineSpec spec);
		~Pipeline();

	private:
		const Device& device;
	};
};