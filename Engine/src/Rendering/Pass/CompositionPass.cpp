#include "CompositionPass.hpp"

#include "Vulkan/ResourceCache.hpp"
#include "Rendering/Renderer.hpp"

#include "Rendering/RenderGraph/RenderGraphCommand.hpp"

namespace Engine
{
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
}