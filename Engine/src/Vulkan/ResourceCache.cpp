#include "ResourceCache.h"

#include "Vulkan/PipelineLayout.h"


namespace Vulkan
{

    ResourceCache::ResourceCache(Device& device) : device(device)
    {
    }

    Engine::ShaderModule& ResourceCache::RequestShader(Engine::ShaderStage stage, const Engine::ShaderSource& source, const Engine::ShaderVariant& variant)
    {
        return shaders.Get(stage, source, variant);
    }

    Vulkan::PipelineLayout& ResourceCache::RequestPipelineLayout(const std::vector<Engine::ShaderModule*>& shaders)
    {
        return pipelineLayouts.Get(device, shaders);
    }

    Pipeline& ResourceCache::RequestPipeline(PipelineState& state)
    {
        return pipelines.Get(device, state);
    }
}
