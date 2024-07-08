#include "CompositionSubpass.hpp"

#include "Vulkan/Caching/ResourceCache.hpp"
#include "Rendering/Renderer.hpp"

namespace Engine
{
    CompositionSubpass::CompositionSubpass(
        RenderContext& renderContext,
        Vulkan::ShaderSource&& vertexSource,
        Vulkan::ShaderSource&& fragmentSource,
        RenderTarget* gBufferTarget
    ) : Subpass{ renderContext, std::move(vertexSource), std::move(fragmentSource) }, gBufferTarget(gBufferTarget)
    {
        auto properties = GetRenderContext().GetDevice().GetPhysicalDeviceProperties();

        VkSamplerCreateInfo sampler = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        sampler.magFilter = VK_FILTER_NEAREST;
        sampler.minFilter = VK_FILTER_NEAREST;
        sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler.mipLodBias = 0.0f;
        sampler.anisotropyEnable = VK_TRUE;
        sampler.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        sampler.minLod = 0.0f;
        sampler.maxLod = 1.0f;
        sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        gBufferSampler = std::make_unique<Vulkan::Sampler>(GetRenderContext().GetDevice(), sampler);
    }

    void CompositionSubpass::Draw(Vulkan::CommandBuffer& commandBuffer)
    {
        commandBuffer.SetRasterizationState({ VK_CULL_MODE_FRONT_BIT });

        auto& view = gBufferTarget->GetViews()[0];

        BindImage(*view, *gBufferSampler, 0, 0, 0);

        auto settings = Renderer::Get().GetSettings();

        auto allocation = GetRenderContext().GetCurrentFrame().RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(HdrSettings));
        allocation.SetData(&settings.hdr);

        BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 1, 0);

        auto& resourceCache = GetRenderContext().GetDevice().GetResourceCache();

        auto& vertexShader = resourceCache.RequestShaderModule(VK_SHADER_STAGE_VERTEX_BIT, GetVertexShader(), {});
        auto& fragmentShader = resourceCache.RequestShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, GetFragmentShader(), {});

        auto& pipelineLayout = GetPipelineLayout({ &vertexShader, &fragmentShader });

        commandBuffer.BindPipelineLayout(pipelineLayout);

        FlushDescriptorSet(commandBuffer, pipelineLayout, 0);

        commandBuffer.Draw(3, 1, 0, 0);
    }
}