#include "ResourceCache.hpp"

#include "Vulkan/PipelineLayout.hpp"

namespace Vulkan
{
    ResourceCache::ResourceCache(Device& device) : device(device)
    {
    }

    Vulkan::ShaderModule& ResourceCache::RequestShaderModule(VkShaderStageFlagBits stage, const ShaderSource& source, const ShaderVariant& variant)
    {
        return shaderModules.Get(device, stage, source, variant);
    }

    Vulkan::PipelineLayout& ResourceCache::RequestPipelineLayout(const std::vector<ShaderModule*>& shaderModules)
    {
        return pipelineLayouts.Get(device, shaderModules);
    }

    Pipeline& ResourceCache::RequestPipeline(PipelineState& state)
    {
        return pipelines.Get(device, state);
    }
}
