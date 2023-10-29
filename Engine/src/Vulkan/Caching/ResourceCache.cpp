#include "ResourceCache.hpp"

#include "Vulkan/Framebuffer.hpp"
#include "Vulkan/PipelineLayout.hpp"

#include "Rendering/RenderTarget.hpp"

namespace Vulkan
{
    ResourceCache::ResourceCache(Device& device) : device(device)
    {
    }

    Framebuffer& ResourceCache::RequestFramebuffer(const RenderPass& renderPass, const Engine::RenderTarget& renderTarget)
    {
        return framebuffers.Get(device, renderPass, renderTarget);

    }

    Vulkan::ShaderModule& ResourceCache::RequestShaderModule(VkShaderStageFlagBits stage, const ShaderSource& source, const ShaderVariant& variant)
    {
        return shaderModules.Get(device, stage, source, variant);
    }

    Vulkan::PipelineLayout& ResourceCache::RequestPipelineLayout(const std::vector<ShaderModule>& shaderModules)
    {
        return pipelineLayouts.Get(device, shaderModules);
    }

    Pipeline& ResourceCache::RequestPipeline(const PipelineLayout& layout, const RenderPass& renderPass, PipelineSpec spec)
    {
        return pipelines.Get(device, layout, renderPass, spec);
    }

}