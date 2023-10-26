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

	struct MultisampleSpec
	{
		VkSampleCountFlagBits rasterizationSamples{ VK_SAMPLE_COUNT_1_BIT };
	};

	struct RasterizationSpec
	{
		VkFrontFace frontFace{ VK_FRONT_FACE_COUNTER_CLOCKWISE };
		VkBool32 depthBiasEnable{ VK_FALSE };
		float depthBiasConstantFactor = 0.0f; 
		float depthBiasClamp = 0.0f;
		float depthBiasSlopeFactor = 0.0f;
	};

	struct PipelineSpec
	{
		VertexInputSpec vertexInput;
		MultisampleSpec multisample;
		RasterizationSpec rasterization;
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