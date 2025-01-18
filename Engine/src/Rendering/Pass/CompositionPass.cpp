#include "CompositionPass.hpp"

#include "Vulkan/ResourceCache.hpp"
#include "Rendering/Renderer.hpp"

#include "Rendering/RenderGraph/RenderGraphCommand.hpp"

namespace Engine
{
    CompositionPass::CompositionPass(
        RenderContext& renderContext,
        ShaderSource&& vertexSource,
        ShaderSource&& fragmentSource,
        RenderTarget* gBufferTarget
    ) : Pass{ renderContext, std::move(vertexSource), std::move(fragmentSource) }, gBufferTarget(gBufferTarget)
    {
        auto properties = GetRenderContext().GetDevice().GetPhysicalDeviceProperties();

        VkSamplerCreateInfo sampler = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        sampler.magFilter = VK_FILTER_NEAREST;
        sampler.minFilter = VK_FILTER_NEAREST;
        sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler.mipLodBias = 0.0f;
        sampler.minLod = 0.0f;
        sampler.maxLod = 1.0f;
        sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        gBufferSampler = std::make_unique<Vulkan::Sampler>(GetRenderContext().GetDevice(), sampler);
    }

    void CompositionPass::RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, CompositionPassData& data)
    {
        auto& gbuffer = context.Get<ForwardPassData>().gbuffer;

        builder.Read(gbuffer, {
            .type = RenderTextureAccessType::Binding,
            .binding = {
                .set = 0,
                .location = 0,
            }
        });

        auto backbuffer = context.Get<BackbufferData>().backbuffer;

        builder.Write(backbuffer, {
            .type = RenderTextureAccessType::Attachment,
            .attachment = {
                .aspect = RenderTextureAspect::Color,
            },
        });
    }

    void CompositionPass::Render(RenderGraphCommand& command, const CompositionPassData& data)
    {
        command.Blit("composition");
    }

    void CompositionPass::Draw(Vulkan::CommandBuffer& commandBuffer)
    {
        commandBuffer.SetRasterizationState({ VK_CULL_MODE_FRONT_BIT });

        auto& attachment = gBufferTarget->GetColorAttachment(0);

        auto& view = attachment.GetResolve() ? attachment.GetResolve()->GetView() : attachment.GetView();

        commandBuffer.BindImage(view, *gBufferSampler, 0, 0, 0);

        auto settings = Renderer::Get().GetSettings();

        auto allocation = GetRenderContext().GetCurrentFrame().RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(HdrSettings));
        allocation.SetData(&settings.hdr);

        commandBuffer.BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 1, 0);

        auto& resourceCache = GetRenderContext().GetDevice().GetResourceCache();

        auto& vertexShader = resourceCache.RequestShader(ShaderStage::Vertex, GetVertexShader(), {});
        auto& fragmentShader = resourceCache.RequestShader(ShaderStage::Fragment, GetFragmentShader(), {});

        auto& pipelineLayout = GetPipelineLayout({ &vertexShader, &fragmentShader });

        commandBuffer.BindPipelineLayout(pipelineLayout);

        commandBuffer.FlushDescriptorSet(0);

        commandBuffer.Draw(3, 1, 0, 0);
    }
}