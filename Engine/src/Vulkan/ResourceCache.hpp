#pragma once

#include "Vulkan/PipelineLayout.hpp"
#include "Vulkan/Pipeline.hpp"
#include "Rendering/Shader.hpp"

template <typename... Args>
inline void Hash(std::size_t& seed, const Vulkan::Device& first, const Args &... args)
{
	Hash(seed, args...);

}

#include "Common/Cache.hpp"

namespace Vulkan
{

	class ResourceCache
	{
	public:
		ResourceCache(Device& device);

		Engine::Shader& RequestShader(Engine::ShaderStage stage, const Engine::ShaderSource& source, const Engine::ShaderVariant& variant);
		PipelineLayout& RequestPipelineLayout(const std::vector<Engine::Shader*>& shaders);
		Pipeline& RequestPipeline(PipelineState& state);
	private:
		Engine::Cache<Engine::Shader> shaders;
		Engine::Cache<PipelineLayout> pipelineLayouts;
		Engine::Cache<Pipeline> pipelines;

		Device& device;
	};
}