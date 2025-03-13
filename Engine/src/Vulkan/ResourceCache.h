#pragma once

#include "Vulkan/PipelineLayout.h"
#include "Vulkan/Pipeline.h"
#include "Rendering/Shader.h"

template <typename... Args>
inline void Hash(std::size_t& seed, const Vulkan::Device& first, const Args &... args)
{
	Hash(seed, args...);

}

#include "Common/Cache.h"

namespace Vulkan
{

	class ResourceCache
	{
	public:
		ResourceCache(Device& device);

		Engine::ShaderModule& RequestShader(Engine::ShaderStage stage, const Engine::ShaderSource& source, const Engine::ShaderVariant& variant);
		PipelineLayout& RequestPipelineLayout(const std::vector<Engine::ShaderModule*>& shaders);
		Pipeline& RequestPipeline(PipelineState& state);
	private:
		Engine::Cache<Engine::ShaderModule> shaders;
		Engine::Cache<PipelineLayout> pipelineLayouts;
		Engine::Cache<Pipeline> pipelines;

		Device& device;
	};
}