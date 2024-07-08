#pragma once

#include "Vulkan/PipelineLayout.hpp"
#include "Vulkan/ShaderModule.hpp"
#include "Vulkan/Pipeline.hpp"

#include "Cache.hpp"

namespace Vulkan
{

	class ResourceCache
	{
	public:
		ResourceCache(Device& device);

		ShaderModule& RequestShaderModule(VkShaderStageFlagBits stage, const ShaderSource& source, const ShaderVariant& variant);
		PipelineLayout& RequestPipelineLayout(const std::vector<ShaderModule*>& shaderModules);
		Pipeline& RequestPipeline(PipelineState& state);
	private:
		Cache<ShaderModule> shaderModules;
		Cache<PipelineLayout> pipelineLayouts;
		Cache<Pipeline> pipelines;

		Device& device;
	};
}