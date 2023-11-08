#pragma once

#include "Vulkan/Framebuffer.hpp"
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

		Framebuffer& RequestFramebuffer(const RenderPass& renderPass, const Engine::RenderTarget& renderTarget);
		ShaderModule& RequestShaderModule(VkShaderStageFlagBits stage, const ShaderSource& source, const ShaderVariant& variant);
		PipelineLayout& RequestPipelineLayout(const std::vector<ShaderModule*>& shaderModules);
		Pipeline& RequestPipeline(PipelineState& state);
			
	private:
		Cache<Framebuffer> framebuffers;
		Cache<ShaderModule> shaderModules;
		Cache<PipelineLayout> pipelineLayouts;
		Cache<Pipeline> pipelines;

		Device& device;
	};
}